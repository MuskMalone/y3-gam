/*!*********************************************************************
\file   PrefabManager.cpp
\author chengen.lau\@digipen.edu
\date   14-September-2024
\brief
  This file contains the definition of PrefabManager singleton.
  It is responsible for the mapping of entities to the prefabs they
  were created from. This is to allow each instance to be updated by
  any changes made to the prefab itself. The functions below are
  used to facilitate the adding and removing of entities to prefabs,
  as well as the updating of components based on prefabs.
  Currently, the PrefabManager attaches a "version" to each prefab so
  that only outdated entities are updated with the prefab's components
  upon loading a scene.
  
  Further improvements can be made so that an entity's component 
  should no longer be updated if it was changed externally through
  inspector.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include <Prefabs/PrefabManager.h>
#include <Serialization/Serializer.h>
#include <Serialization/Deserializer.h>
#include <Events/EventManager.h>
#include <filesystem>
#include <Core/Components/Transform.h>

#ifdef _DEBUG
//#define PREFAB_MANAGER_DEBUG
#endif

using namespace Prefabs;

void PrefabManager::Init()
{
  SUBSCRIBE_CLASS_FUNC(Events::EventType::SPAWN_PREFAB, &PrefabManager::HandleEvent, this);
  SUBSCRIBE_CLASS_FUNC(Events::EventType::DELETE_PREFAB, &PrefabManager::HandleEvent, this);
}

ECS::Entity PrefabManager::SpawnPrefab(const std::string& key, glm::dvec3 const& pos, bool mapEntity)
{
  PrefabDataContainer::const_iterator iter{ mPrefabs.find(key) };
  if (iter == mPrefabs.end()) {
    throw Debug::Exception<PrefabManager>(Debug::LVL_ERROR, Msg("Unable to load prefab " + key));
  }

  auto entityData{ iter->second.Construct(pos) };
  return entityData;
}

Prefab const& PrefabManager::GetVariantPrefab(std::string const& name) const
{
  PrefabDataContainer::const_iterator ret{ mPrefabs.find(name) };
  if (ret == mPrefabs.cend()) {
    throw Debug::Exception<PrefabManager>(Debug::LVL_ERROR, Msg("Unable to find prefab with name: " + name));
  }

  return ret->second;
}

void PrefabManager::ReloadPrefabs()
{
  mPrefabs.clear();
  //LoadPrefabsFromFile();
}

EVENT_CALLBACK_DEF(PrefabManager, HandleEvent)
{
  switch (event->GetCategory())
  {
  case Events::EventType::SPAWN_PREFAB:
  {
    auto pfbEvent{ CAST_TO_EVENT(Events::SpawnPrefabEvent) };
    // if its already loaded, simply create an instance
    if (!IsPrefabLoaded(pfbEvent->mName)) {
      LoadPrefab(pfbEvent->mName);
    }

    SpawnPrefab(pfbEvent->mName, pfbEvent->mPos, pfbEvent->mMapEntity);
    break;
  }
  case Events::EventType::DELETE_PREFAB:
  {
    PrefabDataContainer::const_iterator iter{ mPrefabs.find(std::static_pointer_cast<Events::DeletePrefabEvent>(event)->mName) };
    if (iter != mPrefabs.cend()) { mPrefabs.erase(iter); }
    break;
  }
  }
}

bool PrefabManager::DoesPrefabExist(std::string const& name) const
{
  // if a loaded version exists, return
  if (IsPrefabLoaded(name)) { return true; }

  // else check the Prefabs dir
  for (auto const& file : std::filesystem::recursive_directory_iterator(gPrefabsDirectory)) {
    if (file.path().stem() == name) { return true; }
  }

  return false;
}

void PrefabManager::ReloadPrefab(std::string const& name, std::string const& filePath) {
  mPrefabs[name] = Serialization::Deserializer::DeserializePrefabToVariant(filePath);
}

void PrefabManager::LoadPrefab(std::string const& name) {
  if (IsPrefabLoaded(name)) { return; }

  mPrefabs.emplace(name, Serialization::Deserializer::DeserializePrefabToVariant(gPrefabsDirectory + name + gPrefabFileExt));
}

void PrefabManager::Shutdown() {
  mPrefabs.clear();
}

/*---------------------------- EDITOR - ONLY FUNCTIONS ----------------------------*/
std::pair<ECS::Entity, Prefabs::Prefab::EntityMappings> PrefabManager::SpawnPrefabAndMap(const std::string& key, glm::dvec3 const& pos, bool mapEntity)
{
  PrefabDataContainer::const_iterator iter{ mPrefabs.find(key) };
  if (iter == mPrefabs.end()) {
    throw Debug::Exception<PrefabManager>(Debug::LVL_ERROR, Msg("Unable to load prefab " + key));
  }

  auto entityData{ iter->second.ConstructAndMap(pos) };
  return entityData;
}

void PrefabManager::UpdatePrefabFromEditor(ECS::Entity prefabInstance, std::string const& key,
  Prefabs::Prefab::EntityMappings& mappings, std::string const& filePath)
{
  PrefabDataContainer::iterator iter{ mPrefabs.find(key) };
  if (iter == mPrefabs.end()) {
    Debug::DebugLogger::GetInstance().LogError("[PrefabManager] Trying to update non-existent prefab: " + key);
    return;
  }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  Prefab prefab{ key };
  prefab.mIsActive = true; // entityMan.GetIsActiveEntity(entity);
  prefab.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(prefabInstance);
  if (entityMan.HasChild(prefabInstance)) {
    prefab.CreateFixedSubData(entityMan.GetChildEntity(prefabInstance), mappings);
  }

  Serialization::Serializer::SerializePrefab(prefab, filePath);
  iter->second = std::move(prefab);
}

Prefab PrefabManager::CreateVariantPrefab(ECS::Entity entity, std::string const& name, bool convertToInstance)
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  Prefab prefab{ name };
  prefab.mIsActive = true; // entityMan.GetIsActiveEntity(entity);
  prefab.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(entity);

  if (convertToInstance) {
    entity.EmplaceComponent<Component::PrefabOverrides>(name, Prefabs::PrefabSubData::BasePrefabId);
  }

  if (entityMan.HasChild(entity)) {
    prefab.CreateSubData(entityMan.GetChildEntity(entity), convertToInstance);
  }

  return prefab;
}

void PrefabManager::CreatePrefabFromEntity(ECS::Entity const& entity, std::string const& name,
  std::string const& path, bool convertToInstance)
{
  Prefab prefab{ CreateVariantPrefab(entity, name, convertToInstance) };

  //Assets::AssetManager& am{ Assets::AssetManager::GetInstance() };
  std::string const savePath{ path.empty() ? gPrefabsDirectory + name + gPrefabFileExt : path };
  Serialization::Serializer::SerializePrefab(prefab, savePath);

  //am.ReloadFiles(Assets::AssetType::PREFAB);
  Debug::DebugLogger::GetInstance().LogInfo("Prefab " + name + " saved to " + savePath);
}

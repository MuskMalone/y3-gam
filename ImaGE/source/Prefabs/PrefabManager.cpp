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
#include <Core/Component/Transform.h>

#ifdef _DEBUG
//#define PREFAB_MANAGER_DEBUG
#endif

using namespace Prefabs;

void PrefabManager::Init()
{
  SUBSCRIBE_CLASS_FUNC(Events::EventType::SPAWN_PREFAB, &PrefabManager::HandleEvent, this);
#ifndef IMGUI_DISABLE
  SUBSCRIBE_CLASS_FUNC(Events::EventType::DELETE_PREFAB, &PrefabManager::HandleEvent, this);
#endif
}

ECS::Entity PrefabManager::SpawnPrefab(const std::string& key, glm::dvec3 const& pos, bool mapEntity)
{
  PrefabDataContainer::const_iterator iter{ mPrefabs.find(key) };
  if (iter == mPrefabs.end()) {
    throw Debug::Exception<PrefabManager>(Debug::LVL_ERROR, Msg("Unable to load prefab " + key));
  }

  auto entityData{ iter->second.Construct(pos) };

  if (mapEntity) {
    // set entity's prefab source
    mEntitiesToPrefabs[entityData.first.GetRawEnttEntityID()] = std::move(entityData.second);
  }

  return entityData.first;
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
#ifndef IMGUI_DISABLE
  case Events::EventType::REMOVE_ENTITY:
  {
    EntityPrefabMap::const_iterator iter{ mEntitiesToPrefabs.find(std::static_pointer_cast<Events::RemoveEntityEvent>(event)->mEntityId) };
    if (iter != mEntitiesToPrefabs.cend()) { mEntitiesToPrefabs.erase(iter); }
    break;
  }
  case Events::EventType::DELETE_PREFAB:
  {
    PrefabDataContainer::const_iterator iter{ mPrefabs.find(std::static_pointer_cast<Events::DeletePrefabEvent>(event)->mName) };
    if (iter != mPrefabs.cend()) { mPrefabs.erase(iter); }
    break;
  }
#endif
  }
}


/*---------------------------- EDITOR - ONLY FUNCTIONS ----------------------------*/
#ifndef IMGUI_DISABLE
void PrefabManager::AttachPrefab(ECS::Entity entity, EntityPrefabMap::mapped_type const& prefab)
{
#ifdef PREFAB_MANAGER_DEBUG
  std::cout << "Entity " << entity.GetEntityID() << ": " << prefab.mPrefab << ", version " << prefab.mVersion << "\n";
#endif
  mEntitiesToPrefabs[entity.GetRawEnttEntityID()] = prefab;
}
void PrefabManager::AttachPrefab(ECS::Entity entity, EntityPrefabMap::mapped_type&& prefab)
{
#ifdef PREFAB_MANAGER_DEBUG
  std::cout << "Entity " << entity.GetEntityID() << ": " << prefab.mPrefab << ", version " << prefab.mVersion << "\n";
#endif
  mEntitiesToPrefabs[entity.GetRawEnttEntityID()] = std::move(prefab);
}

void PrefabManager::DetachPrefab(ECS::Entity entity)
{
  EntityPrefabMap::const_iterator entry{ mEntitiesToPrefabs.find(entity.GetRawEnttEntityID()) };
  if (entry == mEntitiesToPrefabs.cend()) { return; }

  // remove entry if it exists
  mEntitiesToPrefabs.erase(entry);
}

std::optional<std::reference_wrapper<PrefabManager::EntityPrefabMap::mapped_type>> PrefabManager::GetEntityPrefab(ECS::Entity entity)
{
  EntityPrefabMap::iterator entry{ mEntitiesToPrefabs.find(entity.GetRawEnttEntityID()) };
  if (entry == mEntitiesToPrefabs.end()) { return std::nullopt; }

  return entry->second;
}

bool PrefabManager::UpdateEntitiesFromPrefab(std::string const& prefab)
{
//  Reflection::ObjectFactory const& of{ Reflection::ObjectFactory::GetInstance() };
//  bool instanceUpdated{ false };
//
//  Prefab const& prefabVar{ GetVariantPrefab(prefab) };
//  for (EntityPrefabMap::iterator iter{ mEntitiesToPrefabs.begin() }; iter != mEntitiesToPrefabs.cend(); ++iter)
//  {
//    EntityPrefabMap::mapped_type& iterVal{ iter->second };
//    ECS::Entity entity{ iter->first };
//
//    // if prefabs name don't match or not registered, continue
//    if (iterVal.mPrefab != prefab || !iterVal.mRegistered) { continue; }
//
//    // if prefab versions match, means its up-to-date so continue
//    if (iterVal.mVersion == prefabVar.mVersion)
//    {
//#ifdef PREFAB_MANAGER_DEBUG
//      std::cout << " Entity " << entity << " matches " << prefab << "'s version of " << prefabVar.mVersion << "\n";
//#endif
//      continue;
//    }
//
//    instanceUpdated = true;
//    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
//    // for parent, update all components except worldPos in transform
//    auto pos{ entity.GetComponent<Component::Transform>().worldPos };
//    of.AddComponentsToEntity(entity, prefabVar.mComponents);
//    entity.GetComponent<Component::Transform>().worldPos = std::move(pos);
//
//    auto& mappedData{ iterVal.mObjToEntity };
//
//    // apply each child removal that is later than current entity version
//    for (auto childIter{ prefabVar.mRemovedChildren.rbegin() };
//      childIter != prefabVar.mRemovedChildren.rend() && childIter->second > iterVal.mVersion; ++childIter)
//    {
//      auto dataIter{ mappedData.find(childIter->first) };
//      if (dataIter == mappedData.end())
//      {
//        std::ostringstream oss{};
//        oss << "Unable to remove obj ID " << childIter->first << " when updating prefab instances of " << prefab;
//        continue;
//      }
//#ifdef PREFAB_MANAGER_DEBUG
//      std::cout << "Updating child deletion: Removed entity " << static_cast<uint32_t>(dataIter->second) << " (id: " << childIter->first << ")\n";
//#endif
//      entityMan.RemoveEntity(dataIter->second);
//      mappedData.erase(dataIter);
//    }
//    iterVal.Validate();
//
//    // apply component removals that are later than current version
//    for (auto compIter{ prefabVar.mRemovedComponents.rbegin() };
//      compIter != prefabVar.mRemovedComponents.rend() && compIter->mVersion > iterVal.mVersion; ++compIter)
//    {
//      auto dataIter{ mappedData.find(compIter->mId) };
//      if (dataIter == mappedData.end())
//      {
//        std::ostringstream oss{};
//        oss << "  Unable to remove " << compIter->mType.get_name().to_string() << " component from obj with ID "
//          << compIter->mId << " when updating prefab instances of " << prefab;
//        continue;
//      }
//
//      ECS::Entity subObj{ dataIter->second };
//#ifdef PREFAB_MANAGER_DEBUG
//      std::cout << "  Updating component deletion: Removed " << compIter->mType.get_name().to_string()
//        << " component from entity " << subObj.GetEntityID() << " (id: " << compIter->mId << ")\n";
//#endif
//      
//      Reflection::ObjectFactory::GetInstance().RemoveComponentFromEntity(subObj, compIter->mType);
//    }
//
//    bool newEntityCreated{ false };
//    for (PrefabSubData const& obj : prefabVar.mObjects)
//    {
//      auto childEntity{ mappedData.find(obj.mId) };
//      if (childEntity == mappedData.cend())
//      {
//#ifdef PREFAB_MANAGER_DEBUG
//        std::cout << "Entity " << static_cast<uint32_t>(iter->first) << " missing child object of id " << obj.mId << ". Creating...\n";
//#endif
//        ECS::Entity const newChild{ obj.Construct() };
//        mappedData.emplace(obj.mId, newChild.GetRawEnttEntityID());
//        newEntityCreated = true;
//      }
//      else
//      {
//        of.AddComponentsToEntity(childEntity->second, obj.mComponents);
//      }
//    }
//
//    // if new entity was created, iterate through all mappings
//    // and re-establish hierarchy
//    if (newEntityCreated)
//    {
//      for (PrefabSubData const& obj : prefabVar.mObjects)
//      {
//        ECS::Entity const child{ mappedData[obj.mId] }, parent{ mappedData[obj.mParent] };
//        if (entityMan.HasParent(child)) {
//          entityMan.RemoveParent(child);
//        }
//        entityMan.SetParentEntity(parent, child);
//#ifdef PREFAB_MANAGER_DEBUG
//        std::cout << "  Set Entity " << parent.GetEntityID() << " as parent of " << child.GetEntityID() << "\n";
//#endif
//      }
//    }
//
//    iterVal.mVersion = prefabVar.mVersion;  // update version of entity
//#ifdef PREFAB_MANAGER_DEBUG
//    std::cout << "  Entity " << static_cast<uint32_t>(iter->first) << " updated with " << prefab << " version " << iterVal.mVersion << "\n";
//#endif
//  }
//
//  return instanceUpdated;
  return false;
}

bool PrefabManager::UpdateAllEntitiesFromPrefab()
{
  bool instanceUpdated{ false };
  for (auto const& [prefab, data] : mPrefabs)
  {
    try {
      if (UpdateEntitiesFromPrefab(prefab)) { instanceUpdated = true; }
    }
    catch (Debug::ExceptionBase& e) {
      e.LogSource();
    }
  }

  return instanceUpdated;
}

void PrefabManager::UpdatePrefabFromEditor(ECS::Entity prefabInstance, std::vector<Prefabs::SubDataId> const& removedChildren,
  std::vector<std::pair<Prefabs::SubDataId, rttr::type>> const& removedComponents, std::string const& filePath)
{
  //PrefabDataContainer::iterator iter{ mPrefabs.find(prefabInstance.GetComponent<Component::Tag>().tag) };
  //if (iter == mPrefabs.end()) {
  //  Debug::DebugLogger::GetInstance().LogError("[PrefabManager] Trying to update non-existent prefab: " + prefabInstance.GetComponent<Component::Tag>().tag);
  //  return;
  //}
  //Prefab& original{ iter->second };
  //Prefab newPrefab{ CreateVariantPrefab(prefabInstance, original.mName) };
  //newPrefab.mVersion = original.mVersion + 1;

  //// update prefab with removed objects
  //for (auto const& elem : removedChildren)
  //{
  //  original.mRemovedChildren.emplace_back(elem, newPrefab.mVersion);
  //}
  //for (auto const& [id, type] : removedComponents)
  //{
  //  original.mRemovedComponents.emplace_back(id, type, newPrefab.mVersion);
  //}
  //newPrefab.mRemovedChildren = std::move(original.mRemovedChildren);
  //newPrefab.mRemovedComponents = std::move(original.mRemovedComponents);

  //Serialization::Serializer::SerializePrefab(newPrefab, filePath);
  //iter->second = std::move(newPrefab);
}

Prefab PrefabManager::CreateVariantPrefab(ECS::Entity entity, std::string const& name)
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  Prefab prefab{ name };
  prefab.mIsActive = true; // entityMan.GetIsActiveEntity(entity);
  prefab.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(entity);
  if (entityMan.HasChild(entity)) {
    prefab.CreateSubData(entityMan.GetChildEntity(entity));
  }

  return prefab;
}

void PrefabManager::CreatePrefabFromEntity(ECS::Entity const& entity, std::string const& name, std::string const& path)
{
  Prefab prefab{ CreateVariantPrefab(entity, name) };

  //Assets::AssetManager& am{ Assets::AssetManager::GetInstance() };
  std::string const savePath{ path.empty() ? gPrefabsDirectory + name + gPrefabFileExt : path };
  Serialization::Serializer::SerializePrefab(prefab, savePath);

  //am.ReloadFiles(Assets::AssetType::PREFAB);
  Debug::DebugLogger::GetInstance().LogInfo("Prefab " + name + " saved to " + savePath);
}
#endif

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

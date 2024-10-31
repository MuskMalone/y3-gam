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
#include <Asset/IGEAssets.h>

#ifdef _DEBUG
//#define PREFAB_MANAGER_DEBUG
#endif

using namespace IGE;
using namespace Prefabs;

namespace {
  void UpdateInstanceGUID(ECS::Entity entity, Assets::GUID guid);
}

PrefabManager::PrefabManager()
{
  SUBSCRIBE_CLASS_FUNC(Events::EventType::SPAWN_PREFAB, &PrefabManager::HandleEvent, this);
}

EVENT_CALLBACK_DEF(PrefabManager, HandleEvent)
{
  switch (event->GetCategory())
  {
  case Events::EventType::SPAWN_PREFAB:
  {
    auto pfbEvent{ CAST_TO_EVENT(Events::SpawnPrefabEvent) };
    Assets::AssetManager& assetMan{ IGE_ASSETMGR };
    Assets::GUID guid{ assetMan.LoadRef<IGE::Assets::PrefabAsset>(pfbEvent->mPath) };
    assetMan.GetAsset<IGE::Assets::PrefabAsset>(guid)->mPrefabData.Construct(guid, pfbEvent->mPos);

    break;
  }
  }
}

bool PrefabManager::DoesPrefabExist(std::string const& name) const
{
  // else check the Prefabs dir
  for (auto const& file : std::filesystem::recursive_directory_iterator(gPrefabsDirectory)) {
    if (file.path().stem() == name) { return true; }
  }

  return false;
}

/*---------------------------- EDITOR - ONLY FUNCTIONS ----------------------------*/
std::pair<ECS::Entity, Prefabs::Prefab::EntityMappings> PrefabManager::SpawnPrefabAndMap(IGE::Assets::GUID guid, glm::dvec3 const& pos, bool mapEntity)
{
  Prefabs::Prefab const& prefab{ IGE_ASSETMGR.GetAsset<IGE::Assets::PrefabAsset>(guid)->mPrefabData };

  auto entityData{ prefab.ConstructAndMap(pos) };
  return entityData;
}

void PrefabManager::UpdatePrefabFromEditor(ECS::Entity prefabInstance, std::string const& key,
  Prefabs::Prefab::EntityMappings& mappings, std::string const& filePath)
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  Prefab prefab{};
  prefab.mIsActive = true; // entityMan.GetIsActiveEntity(entity);
  prefab.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(prefabInstance);
  if (entityMan.HasChild(prefabInstance)) {
    prefab.CreateFixedSubData(entityMan.GetChildEntity(prefabInstance), mappings);
  }

  Serialization::Serializer::SerializePrefab(prefab, filePath);
  // @TODO: Call asset manager to reload prefab here
}

void PrefabManager::CreatePrefabFromEntity(ECS::Entity const& entity, std::string const& name, std::string const& path)
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  Prefab prefab{};
  prefab.mIsActive = true; // entityMan.GetIsActiveEntity(entity);
  prefab.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(entity);

  if (entityMan.HasChild(entity)) {
    prefab.CreateSubData({}, entityMan.GetChildEntity(entity));
  }

  std::string const savePath{ path.empty() ? gPrefabsDirectory + name + gPrefabFileExt : path };
  Serialization::Serializer::SerializePrefab(prefab, path);

  // add the new guid to the PrefabOverrides component
  IGE::Assets::GUID const guid{ IGE_ASSETMGR.ImportAsset<IGE::Assets::PrefabAsset>(path) };
  UpdateInstanceGUID(entity, guid);

  Debug::DebugLogger::GetInstance().LogInfo("Prefab " + name + " saved to " + savePath);
}

namespace {
  void UpdateInstanceGUID(ECS::Entity entity, Assets::GUID guid) {
    entity.GetComponent<Component::PrefabOverrides>().guid = guid;

    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
    if (!entityMan.HasChild(entity)) { return; }

    for (ECS::Entity child : entityMan.GetChildEntity(entity)) {
      UpdateInstanceGUID(child, guid);
    }
  }
}

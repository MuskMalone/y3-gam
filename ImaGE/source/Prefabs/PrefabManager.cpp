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
#include <Events/Events.h>
//#include <ObjectFactory/ObjectFactory.h>

#ifdef _DEBUG
//#define PREFAB_MANAGER_DEBUG
#endif

using namespace Prefabs;

void PrefabManager::LoadPrefabsFromFile()
{
  /*auto const& prefabs{ GE::Assets::AssetManager::GetInstance().GetPrefabs() };
  for (auto const& [name, path] : prefabs)
  {
    try
    {
      m_prefabs.emplace(name, Serialization::Deserializer::DeserializePrefabToVariant(path));
    }
    catch (GE::Debug::IExceptionBase& e)
    {
      e.LogSource();
    }
  }*/
}

ECS::Entity PrefabManager::SpawnPrefab(const std::string& key, glm::dvec3 const& pos, bool mapEntity)
{
  PrefabDataContainer::const_iterator iter{ m_prefabs.find(key) };
  if (iter == m_prefabs.end()) {
    // logger
    throw std::runtime_error("Unable to load prefab " + key);
  }

  auto mappedData{ iter->second.Construct() };
  ECS::Entity newEntity{ ECS::EntityManager::GetInstance().CreateEntityWithTag(key) };
  Component::Transform& trans{ newEntity.GetComponent<Component::Transform>() };
  trans.worldPos = pos;

  if (mapEntity) {
    // set entity's prefab source
    m_entitiesToPrefabs[newEntity.GetRawEnttEntityID()] = std::move(mappedData.second);
  }

  return newEntity;
}

VariantPrefab const& PrefabManager::GetVariantPrefab(std::string const& name) const
{
  PrefabDataContainer::const_iterator ret{ m_prefabs.find(name) };
  if (ret == m_prefabs.cend()) {
    // logger
    throw std::runtime_error("Unable to find prefab with name: " + name);
  }

  return ret->second;
}

void PrefabManager::ReloadPrefab(std::string const& name)
{
  /*auto const& prefabs{ GE::Assets::AssetManager::GetInstance().GetPrefabs() };
  auto path{ GE::Assets::AssetManager::GetInstance().GetPrefabs().find(name) };

  if (path == prefabs.cend())
  {
    throw Debug::Exception<PrefabManager>(Debug::LEVEL_ERROR, ErrMsg("Unable to get path of prefab: " + name));
  }

  m_prefabs[name] = Serialization::Deserializer::DeserializePrefabToVariant(path->second);*/
}

void PrefabManager::ReloadPrefabs()
{
  m_prefabs.clear();
  LoadPrefabsFromFile();
}


/*---------------------------- EDITOR - ONLY FUNCTIONS ----------------------------*/
#ifndef IMGUI_DISABLE
void PrefabManager::AttachPrefab(ECS::Entity entity, EntityPrefabMap::mapped_type const& prefab)
{
#ifdef PREFAB_MANAGER_DEBUG
  std::cout << "Entity " << entity.GetEntityID() << ": " << prefab.mPrefab << ", version " << prefab.mVersion << "\n";
#endif
  m_entitiesToPrefabs[entity.GetRawEnttEntityID()] = prefab;
}
void PrefabManager::AttachPrefab(ECS::Entity entity, EntityPrefabMap::mapped_type&& prefab)
{
#ifdef PREFAB_MANAGER_DEBUG
  std::cout << "Entity " << entity.GetEntityID() << ": " << prefab.mPrefab << ", version " << prefab.mVersion << "\n";
#endif
  m_entitiesToPrefabs[entity.GetRawEnttEntityID()] = std::move(prefab);
}

void PrefabManager::DetachPrefab(ECS::Entity entity)
{
  EntityPrefabMap::const_iterator entry{ m_entitiesToPrefabs.find(entity.GetRawEnttEntityID()) };
  if (entry == m_entitiesToPrefabs.cend()) { return; }

  // remove entry if it exists
  m_entitiesToPrefabs.erase(entry);
}

std::optional<std::reference_wrapper<PrefabManager::EntityPrefabMap::mapped_type>> PrefabManager::GetEntityPrefab(ECS::Entity entity)
{
  EntityPrefabMap::iterator entry{ m_entitiesToPrefabs.find(entity.GetRawEnttEntityID()) };
  if (entry == m_entitiesToPrefabs.end()) { return std::nullopt; }

  return entry->second;
}

bool PrefabManager::UpdateEntitiesFromPrefab(std::string const& prefab)
{
  return false;
//  ObjectFactory::ObjectFactory const& of{ ObjectFactory::ObjectFactory::GetInstance() };
//  rttr::type const transType{ rttr::type::get<Component::Transform*>() };
//  bool instanceUpdated{ false };
//
//  VariantPrefab const& prefabVar{ GetVariantPrefab(prefab) };
//  for (EntityPrefabMap::iterator iter{ m_entitiesToPrefabs.begin() }; iter != m_entitiesToPrefabs.cend(); ++iter)
//  {
//    EntityPrefabMap::mapped_type& iterVal{ iter->second };
//    ECS::Entity const& entity{ iter->first };
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
//    auto pos{ entity.GetComponent<Component::Transform>()->m_pos };
//    of.AddComponentsToEntity(entity, prefabVar.mComponents);
//    entity.GetComponent<Component::Transform>()->m_pos = std::move(pos);
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
//        // log
//        continue;
//      }
//#ifdef PREFAB_MANAGER_DEBUG
//      std::cout << "Updating child deletion: Removed entity " << dataIter->second << " (id: " << childIter->first << ")\n";
//#endif
//      entityMan.DeleteEntity(dataIter->second);
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
//        // log
//        continue;
//      }
//
//#ifdef PREFAB_MANAGER_DEBUG
//      std::cout << "  Updating component deletion: Removed " << compIter->mType 
//        << " component from entity " << dataIter->second << " (id: " << compIter->mId << ")\n";
//#endif
//      ObjectFactory::ObjectFactory::GetInstance().RemoveComponentFromEntity(dataIter->second, compIter->mType);
//    }
//
//    bool newEntityCreated{ false };
//    for (PrefabSubData const& obj : prefabVar.mObjects)
//    {
//      auto childEntity{ mappedData.find(obj.mId) };
//      if (childEntity == mappedData.cend())
//      {
//#ifdef PREFAB_MANAGER_DEBUG
//        std::cout << "Entity " << iter->first << " missing child object: " << obj.mName << ". Creating...\n";
//#endif
//        ECS::Entity const newChild{ obj.Construct() };
//        mappedData.emplace(obj.mId, newChild);
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
//        ecs.SetParentEntity(child, parent);
//        ecs.AddChildEntity(parent, child);
//      }
//    }
//
//    // set all child positions based on parent transform
//    Math::dMat4 const identity
//    {
//      { 1, 0, 0, 0 },
//      { 0, 1, 0, 0 },
//      { 0, 0, 1, 0 },
//      { 0, 0, 0, 1 }
//    };
//    Systems::PostRootTransformSystem::Propergate(entity, identity);
//
//    iterVal.mVersion = prefabVar.mVersion;  // update version of entity
//#ifdef PREFAB_MANAGER_DEBUG
//    std::cout << "  Entity " << iter->first << " updated with " << prefab << " version " << iterVal.mVersion << "\n";
//#endif
//  }
//
//  return instanceUpdated;
}

bool PrefabManager::UpdateAllEntitiesFromPrefab()
{
  bool instanceUpdated{ false };
  /*for (auto const& [prefab, path] : Assets::AssetManager::GetInstance().GetPrefabs())
  {
    try {
      if (UpdateEntitiesFromPrefab(prefab)) { instanceUpdated = true; }
    }
    catch (GE::Debug::IExceptionBase& e) {
      e.LogSource();
    }
  }*/

  return instanceUpdated;
}

void PrefabManager::UpdatePrefabFromEditor(ECS::Entity prefabInstance, std::vector<Prefabs::PrefabSubData::SubDataId> const& removedChildren,
  std::vector<std::pair<Prefabs::PrefabSubData::SubDataId, rttr::type>> const& removedComponents, std::string const& filepath)
{
  //PrefabDataContainer::iterator iter{ m_prefabs.find(ECS::EntityComponentSystem::GetInstance().GetEntityName(prefabInstance)) };
  //if (iter == m_prefabs.end())
  //{
  //  throw Debug::Exception<PrefabManager>(Debug::LEVEL_ERROR, ErrMsg("Trying to update non-existent prefab: "
  //    + ECS::EntityComponentSystem::GetInstance().GetEntityName(prefabInstance)));
  //}
  //VariantPrefab& original{ iter->second };
  //VariantPrefab newPrefab{ CreateVariantPrefab(prefabInstance, original.mName) };
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

  //Serialization::Serializer::SerializeVariantToPrefab(newPrefab, filepath);
  //iter->second = std::move(newPrefab);
}

VariantPrefab PrefabManager::CreateVariantPrefab(ECS::Entity entity, std::string const& name)
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  VariantPrefab prefab{ name };
  prefab.mIsActive = true; // entityMan.GetIsActiveEntity(entity);
  //prefab.mComponents = ObjectFactory::ObjectFactory::GetInstance().GetEntityComponents(entity);
  prefab.CreateSubData(entityMan.GetChildEntity(entity));

  return prefab;
}

void PrefabManager::CreatePrefabFromEntity(ECS::Entity entity, std::string const& name, std::string const& path)
{
  //// if prefab already exists, append "(Copy)" to it
  //std::string prefabName{ name };
  //while (DoesPrefabExist(prefabName))
  //{
  //  prefabName += " (Copy)";
  //}

  //VariantPrefab prefab{ CreateVariantPrefab(entity, prefabName) };

  //Assets::AssetManager& am{ Assets::AssetManager::GetInstance() };
  //if (path.empty()) {
  //  Serialization::Serializer::SerializeVariantToPrefab(prefab,
  //    am.GetConfigData<std::string>("Prefabs Dir") + prefabName + am.GetConfigData<std::string>("Prefab File Extension"));
  //}
  //else {
  //  Serialization::Serializer::SerializeVariantToPrefab(prefab, path);
  //}

  //am.ReloadFiles(Assets::AssetType::PREFAB);
  //ReloadPrefab(prefabName);
 // log prefabName + " saved to Prefabs"
}
#endif

EVENT_CALLBACK_DEF(PrefabManager, HandleEvent)
{
  switch (event->GetCategory())
  {
#ifndef IMGUI_DISABLE
  case Events::EventType::UNLOAD_SCENE:
  {
    m_entitiesToPrefabs.clear();
    break;
  }
  case Events::EventType::REMOVE_ENTITY:
  {
    EntityPrefabMap::const_iterator iter{ m_entitiesToPrefabs.find(std::static_pointer_cast<Events::RemoveEntityEvent>(event)->mEntityId) };
    if (iter != m_entitiesToPrefabs.cend()) { m_entitiesToPrefabs.erase(iter); }
    break;
  }
  case Events::EventType::DELETE_PREFAB:
  {
    PrefabDataContainer::const_iterator iter{ m_prefabs.find(std::static_pointer_cast<Events::DeletePrefabEvent>(event)->mName) };
    if (iter != m_prefabs.cend()) { m_prefabs.erase(iter); }
    break;
  }
#endif
  }
}

/*!*********************************************************************
\file   Prefab.cpp
\author chengen.lau\@digipen.edu
\date   16-September-2024
\brief
  Contains the definition of the struct encapsulating deserialized
  prefab data. It is used during creation of entities from prefabs and
  when editing prefabs in the prefab editor. The implementation makes
  use of RTTR library to store components as rttr::variant objects.
  Each prefab also allows for multiple layers of components.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Prefab.h"
#include <Reflection/ObjectFactory.h>
#include <Core/Components/PrefabOverrides.h>
#include <Core/Components/Transform.h>

using namespace Prefabs;

PrefabSubData::PrefabSubData() : mParent{ BasePrefabId } {}

PrefabSubData::PrefabSubData(SubDataId id, SubDataId parent) :
  mComponents{}, mId{ id }, mParent{ parent } {}

rttr::variant PrefabSubData::GetComponent(rttr::type const& type) const {
  for (rttr::variant const& component : mComponents) {
    if (component.get_type() == type) {
      return component;
    }
  }

  return {};
}

ECS::Entity PrefabSubData::Construct(IGE::Assets::GUID guid, bool createInst) const
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  ECS::Entity entity{ entityMan.CreateEntity() };

  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);
  if (createInst) {
    entity.EmplaceComponent<Component::PrefabOverrides>(guid, mId);
  }

  return entity;
}

Prefab::Prefab() : mName{}, mObjects{} {}

Prefab::Prefab(std::string name) : mName{ std::move(name) }, mObjects{} {}

std::pair<ECS::Entity, Prefab::EntityMappings> Prefab::ConstructAndMap(glm::vec3 const& pos) const
{
  std::unordered_map<SubDataId, ECS::Entity> idsToEntities;
  EntityMappings mappedData{};
  size_t const numObjs{ mObjects.size() };
  idsToEntities.reserve(numObjs);
  mappedData.Reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity root{ entityMan.CreateEntity() };
  {
    Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(root, GetRoot().mComponents);
    Component::Transform& trans{ root.GetComponent<Component::Transform>() };
    trans.worldPos = trans.position = pos;
    //root.EmplaceComponent<Component::PrefabOverrides>(guid);

    // map base ID to this entity ID
    idsToEntities.emplace(PrefabSubData::BasePrefabId, root);
    mappedData.Insert(root.GetRawEnttEntityID(), PrefabSubData::BasePrefabId);
  }

  // then, create child entities and map IDs
  for (unsigned i{ 1 }; i < mObjects.size(); ++i) {
    PrefabSubData const& obj{ mObjects[i] };
    ECS::Entity const subEntity{ obj.Construct({}, false) };

    idsToEntities.emplace(obj.mId, subEntity);
    mappedData.Insert(subEntity.GetRawEnttEntityID(), obj.mId);
  }

  // establish the hierarchy
  for (unsigned i{ 1 }; i < mObjects.size(); ++i) {
    PrefabSubData const& obj{ mObjects[i] };

    ECS::Entity& child{ idsToEntities[obj.mId] }, parent{ idsToEntities[obj.mParent] };
    entityMan.SetParentEntity(parent, child);
  }

  return { root, mappedData };
}

ECS::Entity Prefab::Construct(IGE::Assets::GUID guid, glm::vec3 const& pos) const
{
  std::unordered_map<SubDataId, ECS::Entity> idsToEntities;
  size_t const numObjs{ mObjects.size() };
  idsToEntities.reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity root{ entityMan.CreateEntity() };
  {
    Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(root, GetRoot().mComponents);
    Component::Transform& trans{ root.GetComponent<Component::Transform>() };
    trans.SetLocalToWorld();
    trans.worldPos = trans.position = pos;
    root.EmplaceComponent<Component::PrefabOverrides>(guid);

    // map base ID to this entity ID
    idsToEntities.emplace(PrefabSubData::BasePrefabId, root);
  }

  // then, create child entities and map IDs
  for (unsigned i{ 1 }; i < mObjects.size(); ++i) {
    PrefabSubData const& obj{ mObjects[i] };

    ECS::Entity const subEntity{ obj.Construct(guid) };
    idsToEntities.emplace(obj.mId, subEntity);
  }

  // establish the hierarchy
  for (unsigned i{ 1 }; i < mObjects.size(); ++i) {
    PrefabSubData const& obj{ mObjects[i] };

    ECS::Entity& child{ idsToEntities[obj.mId] }, parent{ idsToEntities[obj.mParent] };
    entityMan.SetParentEntity(parent, child);
  }

  return root;
}

void Prefab::FillPrefabInstance(IGE::Assets::GUID guid, std::unordered_map<Prefabs::SubDataId, ECS::Entity>& idToEntity) const
{
  Reflection::ObjectFactory& of{ Reflection::ObjectFactory::GetInstance() };
  ECS::Entity root{ idToEntity.at(PrefabSubData::BasePrefabId) };
  std::vector<PrefabSubData const*> newSubData;
  
  of.AddComponentsToEntity(root, GetRoot().mComponents);

  // iterate through sub-objects and fill in their corresponding entity's components
  // also construct any missing entities
  for (unsigned i{ 1 }; i < mObjects.size(); ++i) {
    PrefabSubData const& obj{ mObjects[i] };

    // if the prefab instance already had this sub-obj
    if (idToEntity.contains(obj.mId)) {
      of.AddComponentsToEntity(idToEntity[obj.mId], obj.mComponents);
    }
    // else construct the entity
    else {
      ECS::Entity subEntity{ obj.Construct(guid) };
      idToEntity.emplace(obj.mId, subEntity);
      newSubData.emplace_back(&obj);
    }
  }

  if (newSubData.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // establish the hierarchy for new entities
  for (unsigned i{ 1 }; i < newSubData.size(); ++i) {
    PrefabSubData const* obj{ newSubData[i] };

    // dont have to check here because every sub-entity should have a parent
    ECS::Entity const& child{ idToEntity[obj->mId] }, parent{ idToEntity[obj->mParent] };
    entityMan.SetParentEntity(parent, child);
  }
}

void Prefab::CreateSubData(IGE::Assets::GUID guid, std::vector<ECS::Entity> const& children, SubDataId parent)
{
  if (children.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  
  for (ECS::Entity child : children)
  {
    SubDataId const currId{ static_cast<SubDataId>(mObjects.size()) };
    PrefabSubData obj{ currId, parent };

    obj.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(child);

    mObjects.emplace_back(std::move(obj));

    // add prefabOverrides if we are assigning to an instance
    child.EmplaceComponent<Component::PrefabOverrides>(guid, currId);

    if (entityMan.HasChild(child)) {
      CreateSubData(guid, entityMan.GetChildEntity(child), currId);
    }
  }
}

void Prefab::CreateFixedSubData(std::vector<ECS::Entity> const& children, EntityMappings& mappings, SubDataId parent)
{
  if (children.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  for (ECS::Entity const& child : children)
  {
    SubDataId currId;
    // if mapping exists, use previous Id
    if (mappings.Contains(child.GetRawEnttEntityID())) {
      currId = mappings.Get(child.GetRawEnttEntityID());
    }
    // else, we use the next available number
    else {
      currId = static_cast<SubDataId>(mappings.Size());
      while (mappings.Contains(currId)) { ++currId; }
      mappings.Insert(child.GetRawEnttEntityID(), currId);
    }
    PrefabSubData obj{ currId, parent };

    obj.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(child);

    mObjects.emplace_back(std::move(obj));
    if (entityMan.HasChild(child)) {
      CreateFixedSubData(entityMan.GetChildEntity(child), mappings, mappings.Get(child.GetRawEnttEntityID()));
    }
  }
}

void Prefab::Clear() noexcept {
  mObjects.clear();
  mName.clear();
}

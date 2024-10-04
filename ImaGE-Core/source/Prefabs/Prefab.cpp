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
  mComponents{}, mId{ id }, mParent{ parent }, mIsActive{ true } {}

ECS::Entity PrefabSubData::Construct(std::string const& name) const
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  ECS::Entity entity{ entityMan.CreateEntity() };

  //entityMan.SetIsActiveEntity(entity, mIsActive);
  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);
  entity.EmplaceComponent<Component::PrefabOverrides>(name, mId);

  return entity;
}


Prefab::Prefab(std::string name) : mName{ std::move(name) }, mObjects{}, mComponents{}, mIsActive{ true } {}

std::pair<ECS::Entity, Prefab::EntityMappings> Prefab::ConstructAndMap(glm::vec3 const& pos) const
{
  std::unordered_map<SubDataId, ECS::Entity> idsToEntities;
  EntityMappings mappedData{};
  size_t const numObjs{ mObjects.size() + 1 };
  idsToEntities.reserve(numObjs);
  mappedData.Reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity entity{ entityMan.CreateEntityWithTag(mName) };
  //entityMan.SetIsActiveEntity(entity, mIsActive);
  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);
  Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
  trans.worldPos = trans.position = pos;
  entity.EmplaceComponent<Component::PrefabOverrides>(mName);

  // map base ID to this entity ID
  idsToEntities.emplace(PrefabSubData::BasePrefabId, entity);  
  mappedData.Insert(entity.GetRawEnttEntityID(), PrefabSubData::BasePrefabId);

  // then, create child entities and map IDs
  for (PrefabSubData const& obj : mObjects)
  {
    ECS::Entity const subEntity{ obj.Construct(mName) };
    idsToEntities.emplace(obj.mId, subEntity);
    mappedData.Insert(subEntity.GetRawEnttEntityID(), obj.mId);
  }

  // establish the hierarchy
  for (PrefabSubData const& obj : mObjects)
  {
    ECS::Entity const& child{ idsToEntities[obj.mId] }, parent{ idsToEntities[obj.mParent] };
    entityMan.SetParentEntity(parent, child);
    //entityMan.SetIsActiveEntity(child, obj.mIsActive);
  }

  return { entity, mappedData };
}

ECS::Entity Prefab::Construct(glm::vec3 const& pos) const
{
  std::unordered_map<SubDataId, ECS::Entity> idsToEntities;
  size_t const numObjs{ mObjects.size() + 1 };
  idsToEntities.reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity entity{ entityMan.CreateEntityWithTag(mName) };
  //entityMan.SetIsActiveEntity(entity, mIsActive);
  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);
  Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
  trans.worldPos = trans.position = pos;
  entity.EmplaceComponent<Component::PrefabOverrides>(mName);

  // map base ID to this entity ID
  idsToEntities.emplace(PrefabSubData::BasePrefabId, entity);

  // then, create child entities and map IDs
  for (PrefabSubData const& obj : mObjects)
  {
    ECS::Entity const subEntity{ obj.Construct(mName) };
    idsToEntities.emplace(obj.mId, subEntity);
  }

  // establish the hierarchy
  for (PrefabSubData const& obj : mObjects)
  {
    ECS::Entity const& child{ idsToEntities[obj.mId] }, parent{ idsToEntities[obj.mParent] };
    entityMan.SetParentEntity(parent, child);
    //entityMan.SetIsActiveEntity(child, obj.mIsActive);
  }

  return entity;
}

void Prefab::FillPrefabInstance(std::unordered_map<Prefabs::SubDataId, ECS::Entity>& idToEntity) const
{
  Reflection::ObjectFactory& of{ Reflection::ObjectFactory::GetInstance() };
  ECS::Entity entity{ idToEntity.at(PrefabSubData::BasePrefabId) };
  std::vector<PrefabSubData const*> newSubData;
  
  of.AddComponentsToEntity(entity, mComponents);

  // iterate through sub-objects and fill in their corresponding entity's components
  // also construct any missing entities
  for (PrefabSubData const& obj : mObjects)
  {
    // if the prefab instance already had this sub-obj
    if (idToEntity.contains(obj.mId)) {
      of.AddComponentsToEntity(idToEntity[obj.mId], obj.mComponents);
    }
    // else construct the entity
    else {
      ECS::Entity subEntity{ obj.Construct(mName) };
      idToEntity.emplace(obj.mId, subEntity);
      newSubData.emplace_back(&obj);
    }
  }

  if (newSubData.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // establish the hierarchy for new entities
  for (PrefabSubData const* obj : newSubData) {
    // dont have to check here because every sub-entity should have a parent
    ECS::Entity const& child{ idToEntity[obj->mId] }, parent{ idToEntity[obj->mParent] };
    entityMan.SetParentEntity(parent, child);
  }
}

void Prefab::CreateSubData(std::vector<ECS::Entity> const& children, bool assignInstance, SubDataId parent)
{
  if (children.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  
  for (ECS::Entity child : children)
  {
    SubDataId const currId{ static_cast<SubDataId>(mObjects.size() + 1) };
    PrefabSubData obj{ currId, parent };
    obj.mIsActive = true; // entityMan.GetIsActiveEntity(child);

    obj.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(child);

    mObjects.emplace_back(std::move(obj));

    // add prefabOverrides if we are assigning to an instance
    if (assignInstance) {
      child.EmplaceComponent<Component::PrefabOverrides>(mName, currId);
    }

    if (entityMan.HasChild(child)) {
      CreateSubData(entityMan.GetChildEntity(child), currId);
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
    obj.mIsActive = true; // entityMan.GetIsActiveEntity(child);

    obj.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(child);

    mObjects.emplace_back(std::move(obj));
    if (entityMan.HasChild(child)) {
      CreateFixedSubData(entityMan.GetChildEntity(child), mappings, mappings.Get(child.GetRawEnttEntityID()));
    }
  }
}

void Prefab::Clear() noexcept
{
  mName.clear();
  mComponents.clear();
  mObjects.clear();
}

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
#include <Core/Component/PrefabOverrides.h>
#include <Core/Component/Transform.h>

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

std::pair<ECS::Entity, Prefab::EntityMappings> Prefab::Construct(glm::vec3 const& pos) const
{
  std::unordered_map<SubDataId, ECS::Entity> idsToEntities;
  EntityMappings mappedData{};
  size_t const numObjs{ mObjects.size() + 1 };
  idsToEntities.reserve(numObjs);
  mappedData.reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity entity{ entityMan.CreateEntityWithTag(mName) };
  //entityMan.SetIsActiveEntity(entity, mIsActive);
  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);
  Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
  trans.worldPos = trans.localPos = pos;
  entity.EmplaceComponent<Component::PrefabOverrides>(mName);

  // map base ID to this entity ID
  idsToEntities.emplace(PrefabSubData::BasePrefabId, entity);  
  mappedData.emplace(PrefabSubData::BasePrefabId, entity);

  // then, create child entities and map IDs
  for (PrefabSubData const& obj : mObjects)
  {
    ECS::Entity const subEntity{ obj.Construct(mName) };
    idsToEntities.emplace(obj.mId, subEntity);
    mappedData.emplace(obj.mId, subEntity);
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

void Prefab::CreateSubData(std::vector<ECS::Entity> const& children, SubDataId parent)
{
  if (children.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  
  for (ECS::Entity const& child : children)
  {
    SubDataId const currId{ static_cast<SubDataId>(mObjects.size() + 1) };
    PrefabSubData obj{ currId, parent };
    obj.mIsActive = true; // entityMan.GetIsActiveEntity(child);

    obj.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(child);

    mObjects.emplace_back(std::move(obj));
    if (entityMan.HasChild(child)) {
      CreateSubData(entityMan.GetChildEntity(child), currId);
    }
  }
}

void Prefab::Clear() noexcept
{
  mName.clear();
  mComponents.clear();
  mObjects.clear();
}

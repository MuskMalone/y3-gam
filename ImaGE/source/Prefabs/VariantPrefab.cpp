/*!*********************************************************************
\file   VariantPrefab.cpp
\author chengen.lau\@digipen.edu
\date   14-September-2024
\brief
  Contains the definition of the struct encapsulating deserialized
  prefab data. It is used during creation of entities from prefabs and
  when editing prefabs in the prefab editor. The implementation makes
  use of RTTR library to store components as rttr::variant objects.
  Each prefab also allows for multiple layers of components.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "VariantPrefab.h"
#include <Reflection/ObjectFactory.h>

using namespace Prefabs;

PrefabSubData::PrefabSubData() : mParent{ BasePrefabId } {}

PrefabSubData::PrefabSubData(std::string name, SubDataId id, SubDataId parent) :
  mName{ std::move(name) }, mComponents{}, mId{ id }, mParent{ parent } {}

ECS::Entity PrefabSubData::Construct() const
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  ECS::Entity const entity{ entityMan.CreateEntityWithTag(mName) };

  //entityMan.SetIsActiveEntity(entity, mIsActive);
  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);

  return entity;
}

VariantPrefab::VariantPrefab(std::string name, unsigned version) :
  mName{ std::move(name) }, mObjects{}, mComponents{},
  mRemovedChildren{}, mRemovedComponents{}, mVersion { version } {}

void VariantPrefab::Clear() noexcept
{
  mName.clear();
  mComponents.clear();
  mObjects.clear();
  mRemovedChildren.clear();
  mVersion = 0;
}

std::pair<ECS::Entity, VariantPrefab::EntityMappings> VariantPrefab::Construct() const
{
  std::unordered_map<PrefabSubData::SubDataId, ECS::Entity> idsToEntities;
  EntityMappings mappedData{ mName, mVersion };
  size_t const numObjs{ mObjects.size() + 1 };
  idsToEntities.reserve(numObjs);
  mappedData.mObjToEntity.reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity const entity{ entityMan.CreateEntityWithTag(mName) };
  //entityMan.SetIsActiveEntity(entity, mIsActive);
  Reflection::ObjectFactory::GetInstance().AddComponentsToEntity(entity, mComponents);

  // map base ID to this entity ID
  idsToEntities.emplace(PrefabSubData::BasePrefabId, entity);  
  mappedData.mObjToEntity.emplace(PrefabSubData::BasePrefabId, entity.GetRawEnttEntityID());

  // then, create child entities and map IDs
  for (PrefabSubData const& obj : mObjects)
  {
    ECS::Entity const subEntity{ obj.Construct() };
    idsToEntities.emplace(obj.mId, subEntity);
    mappedData.mObjToEntity.emplace(obj.mId, subEntity.GetRawEnttEntityID());
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

void VariantPrefab::CreateSubData(std::vector<ECS::Entity> const& children, PrefabSubData::SubDataId parent)
{
  if (children.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  
  for (ECS::Entity const& child : children)
  {
    PrefabSubData::SubDataId const currId{ static_cast<PrefabSubData::SubDataId>(mObjects.size() + 1) };
    PrefabSubData obj{ child.GetComponent<Component::Tag>().tag, currId, parent};
    obj.mIsActive = true; // entityMan.GetIsActiveEntity(child);

    obj.mComponents = Reflection::ObjectFactory::GetInstance().GetEntityComponents(child);

    rttr::type const transType{ rttr::type::get<Component::Transform*>() };
    for (rttr::variant& comp : obj.mComponents)
    {
      if (comp.get_type().get_wrapped_type() == transType)
      {
        Component::Transform& trans{ *comp.get_value<Component::Transform*>() };
        trans.worldPos = trans.localPos;
        trans.worldRot = trans.localRot;
        trans.worldScale = trans.localScale;
        break;
      }
    }

    mObjects.emplace_back(std::move(obj));
    CreateSubData(entityMan.GetChildEntity(const_cast<ECS::Entity&>(child)), currId);
  }
}

void VariantPrefab::EntityMappings::Validate()
{
  //ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  //for (auto iter{ mObjToEntity.begin() }; iter != mObjToEntity.end();)
  //{
  //  // if entity destroyed, remove entry from map
  //  // checking if component signature is 0 to determine
  //  if (ecs.GetComponentSignature(iter->second).none())
  //  {
  //    iter = mObjToEntity.erase(iter);
  //    continue;
  //  }
  //  ++iter;
  //}
}

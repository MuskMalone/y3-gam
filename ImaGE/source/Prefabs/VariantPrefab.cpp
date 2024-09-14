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
//#include <ObjectFactory/ObjectFactory.h>

using namespace Prefabs;

PrefabSubData::PrefabSubData() : m_parent{ BasePrefabId } {}

PrefabSubData::PrefabSubData(std::string name, SubDataId id, SubDataId parent) :
  m_name{ std::move(name) }, m_components{}, m_id{ id }, m_parent{ parent } {}

ECS::Entity PrefabSubData::Construct() const
{
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  ECS::Entity const entity{ entityMan.CreateEntityWithTag(m_name) };

  //entityMan.SetIsActiveEntity(entity, m_isActive);
  //ObjectFactory::ObjectFactory::GetInstance().AddComponentsToEntity(entity, m_components);

  return entity;
}

VariantPrefab::VariantPrefab(std::string name, unsigned version) :
  m_name{ std::move(name) }, m_objects{}, m_components{},
  m_removedChildren{}, m_removedComponents{}, m_version { version } {}

void VariantPrefab::Clear() noexcept
{
  m_name.clear();
  m_components.clear();
  m_objects.clear();
  m_removedChildren.clear();
  m_version = 0;
}

std::pair<ECS::Entity, VariantPrefab::EntityMappings> VariantPrefab::Construct() const
{
  std::unordered_map<PrefabSubData::SubDataId, ECS::Entity> idsToEntities;
  EntityMappings mappedData{ m_name, m_version };
  size_t const numObjs{ m_objects.size() + 1 };
  idsToEntities.reserve(numObjs);
  mappedData.m_objToEntity.reserve(numObjs);
  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

  // first, create the base entity
  ECS::Entity const entity{ entityMan.CreateEntityWithTag(m_name) };
  //entityMan.SetIsActiveEntity(entity, m_isActive);
  //ObjectFactory::ObjectFactory::GetInstance().AddComponentsToEntity(entity, m_components);

  // map base ID to this entity ID
  idsToEntities.emplace(PrefabSubData::BasePrefabId, entity);  
  mappedData.m_objToEntity.emplace(PrefabSubData::BasePrefabId, entity);

  // then, create child entities and map IDs
  for (PrefabSubData const& obj : m_objects)
  {
    ECS::Entity const subId{ obj.Construct() };
    idsToEntities.emplace(obj.m_id, subId);
    mappedData.m_objToEntity.emplace(obj.m_id, subId);
  }

  // establish the hierarchy
  for (PrefabSubData const& obj : m_objects)
  {
    ECS::Entity const& child{ idsToEntities[obj.m_id] }, parent{ idsToEntities[obj.m_parent] };
    entityMan.SetParentEntity(parent, child);
    //entityMan.SetIsActiveEntity(child, obj.m_isActive);
  }

  return { entity, mappedData };
}

void VariantPrefab::CreateSubData(std::vector<ECS::Entity> const& children, PrefabSubData::SubDataId parent)
{
  if (children.empty()) { return; }

  ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  
  for (ECS::Entity const& child : children)
  {
    PrefabSubData::SubDataId const currId{ static_cast<PrefabSubData::SubDataId>(m_objects.size() + 1) };
    PrefabSubData obj{ child.GetComponent<Component::Tag>().tag, currId, parent};
    obj.m_isActive = true; // entityMan.GetIsActiveEntity(child);

    //obj.m_components = ObjectFactory::ObjectFactory::GetInstance().GetEntityComponents(child);

    rttr::type const transType{ rttr::type::get<Component::Transform*>() };
    for (rttr::variant& comp : obj.m_components)
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

    m_objects.emplace_back(std::move(obj));
    CreateSubData(entityMan.GetChildEntity(const_cast<ECS::Entity&>(child)), currId);
  }
}

void VariantPrefab::EntityMappings::Validate()
{
  //ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
  //for (auto iter{ m_objToEntity.begin() }; iter != m_objToEntity.end();)
  //{
  //  // if entity destroyed, remove entry from map
  //  // checking if component signature is 0 to determine
  //  if (ecs.GetComponentSignature(iter->second).none())
  //  {
  //    iter = m_objToEntity.erase(iter);
  //    continue;
  //  }
  //  ++iter;
  //}
}

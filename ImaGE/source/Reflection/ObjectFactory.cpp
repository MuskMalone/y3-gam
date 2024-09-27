/*!*********************************************************************
\file       ObjectFactory.cpp
\author     chengen.lau\@digipen.edu
\date       15-September-2024
\brief
  Contains the definition of the ObjectFactory singleton class, which
  encapsulates functions and data members to facilitate the creation
  of objects in the scene. The class serves as a middle-man between
  serialization and the actual entities in the scene. It is responsible
  for the creation/cloning of entities and adding of components through
  the ECS.

  On load, the ObjectFactory holds the deserialized data for the scene.
  Upon reloading, the objects are loaded from here without the need to
  deserialize again. Only when changing scenes will a full reload be
  required.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include "pch.h"
#include "ObjectFactory.h"
#include "ComponentTypes.h"
#include <Serialization/Deserializer.h>
#include <sstream>
#include <Physics/PhysicsSystem.h>
#include <TempScene.h> //tch for testing to remove

#ifndef IMGUI_DISABLE
#include <Prefabs/PrefabManager.h>
#include <Events/EventManager.h>
#endif

namespace Reflection
{

  void ObjectFactory::AddComponentsToEntity(ECS::Entity id, std::vector<rttr::variant> const& components) const
  {
    for (rttr::variant const& component : components) {
      AddComponentToEntity(id, component.get_type(), component);
    }
  }

  std::vector<rttr::variant> ObjectFactory::GetEntityComponents(ECS::Entity const& id) const
  {
    std::vector<rttr::variant> ret{};
    
    for (rttr::type const& type : Reflection::gComponentTypes) {
      auto compVar{ GetEntityComponent(id, type) };
      if (compVar) { ret.emplace_back(std::move(compVar)); }
    }

    return ret;
  }

  void ObjectFactory::CloneObject(ECS::Entity const& entity, ECS::Entity const& parent) const
  {
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

    std::string const& name{ entity.GetComponent<Component::Tag>().tag };
    ECS::Entity newEntity{ entityMan.CreateEntityWithTag(name + " (Copy)") };
    //entityMan.SetIsActiveEntity(newEntity, entityMan.GetIsActiveEntity(entity));

    std::vector<rttr::variant> const components{ GetEntityComponents(entity) };

    AddComponentsToEntity(newEntity, components);

    // set parent/child
    entityMan.SetParentEntity(parent, newEntity);
    if (parent) { entityMan.SetChildEntity(parent, newEntity); }
    for (ECS::Entity const& child : entityMan.GetChildEntity(entity)) {
      CloneObject(child, newEntity);  // recursively clone all children
    }
  }

  void ObjectFactory::LoadPrefabInstances() {
    Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };
    for (auto const&[pfb, data] : mPrefabInstances) {
      pm.LoadPrefab(pfb);
      // create an instance of the original
      auto mappings{ pm.GetVariantPrefab(pfb).GetSubObjectComponentMappings() };

      // for every instance, create an entity with its ID,
      // use the mappings to get the referenced subdata and
      // add its components to it, overidding as necessary
      // afterwards, iterate once more to establish hierarchy
      
      // override components of each entity if necessary
      //for (Reflection::PrefabInst const& inst : data) {
      //  if (inst.mPosition) {
      //    Component::Transform& trans{ mappings.first.GetComponent<Component::Transform>() };
      //    trans.worldPos = trans.localPos = *inst.mPosition;
      //  }

      //  // each entity's PrefabOverrides component should 
      //  // contain an id that corresponds to a sub-object
      //  Component::PrefabOverrides const& overrides{ inst.mOverrides };
      //  ECS::Entity& currEntity{ mappings.second[overrides.subDataId] };
      //  currEntity.EmplaceOrReplaceComponent<Component::PrefabOverrides>(overrides);  // restore its PrefabOverrides

      //  // replace any modified components
      //  if (!overrides.modifiedComponents.empty()) {
      //    for (auto const&[compType, compVar] : overrides.modifiedComponents) {
      //      AddComponentToEntity(currEntity, compType, compVar);
      //    }
      //  }
      //  // remove components if necessary
      //  if (!overrides.removedComponents.empty()) {
      //    for (rttr::variant const& comp : overrides.modifiedComponents) {
      //      RemoveComponentFromEntity(currEntity, comp.get_type());
      //    }
      //  }
      //}
    }
  }

  void ObjectFactory::InitScene()
  {
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

    // iterate through data and create entities
    for (auto const& data : mRawEntities)
    {
      ECS::Entity newEntity{ entityMan.CreateEntityWithID({}, data.mID) };
      //entityMan.SetIsActiveEntity(id, arg.mIsActive);
      AddComponentsToEntity(newEntity, data.mComponents);
    }

    // restore the hierarchy
    for (auto const& data : mRawEntities)
    {
      if (data.mParent == entt::null) { continue; }

      entityMan.SetParentEntity(data.mParent, data.mID);
      /*for (ECS::Entity const& child : data.mChildEntities) {
        entityMan.SetChildEntity(data.mID, child);
      }*/
    }

    LoadPrefabInstances();
  }

  void ObjectFactory::ClearData() {
    mRawEntities.clear();
    mPrefabInstances.clear();
  }

  void ObjectFactory::LoadEntityData(std::string const& filePath) {
    Serialization::Deserializer::DeserializeScene(mRawEntities, mPrefabInstances, filePath);
  }

  void ObjectFactory::AddComponentToEntity(ECS::Entity entity, rttr::type const& type, rttr::variant const& compVar) const
  {
    rttr::type compType{ type };
    // get underlying type if it's wrapped in a pointer
    compType = compType.is_wrapper() ? compType.get_wrapped_type().get_raw_type() : compType.is_pointer() ? compType.get_raw_type() : compType;

    if (compType == rttr::type::get<Component::Tag>()) {
      entity.EmplaceOrReplaceComponent<Component::Tag>(*(compVar ? compVar : type.create()).get_value<std::shared_ptr<Component::Tag>>());
    }
    else if (compType == rttr::type::get<Component::Transform>()) {
      entity.EmplaceOrReplaceComponent<Component::Transform>(*(compVar ? compVar : type.create()).get_value<std::shared_ptr<Component::Transform>>());
    }
    else if (compType == rttr::type::get<Component::Layer>()) {
      entity.EmplaceOrReplaceComponent<Component::Layer>(*(compVar ? compVar : type.create()).get_value<std::shared_ptr<Component::Layer>>());
    }
    else if (compType == rttr::type::get<Component::Mesh>()) {
      Scene::AddMesh(entity);
    }
    else if (compType == rttr::type::get<Component::RigidBody>()) {
      IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(entity);
    }
    else if (compType == rttr::type::get<Component::Collider>()) {
      IGE::Physics::PhysicsSystem::GetInstance()->AddCollider(entity);
    }
    else
    {
      std::ostringstream oss{};
      oss << "Trying to add unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::AddComponentToEntity";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
    }
  }

  #define IF_GET_ENTITY_COMP(ComponentClass) if (compType == rttr::type::get<ComponentClass>()) {\
    return entity.HasComponent<ComponentClass>() ? std::make_shared<ComponentClass>(entity.GetComponent<ComponentClass>()) : rttr::variant(); }

  rttr::variant ObjectFactory::GetEntityComponent(ECS::Entity const& entity, rttr::type const& compType) const
  {
    IF_GET_ENTITY_COMP(Component::Transform)
    else IF_GET_ENTITY_COMP(Component::Tag)
    else IF_GET_ENTITY_COMP(Component::Layer)
    //else IF_GET_ENTITY_COMP(Component::Mesh)
    //else IF_GET_ENTITY_COMP(Component::RigidBody)
    //else IF_GET_ENTITY_COMP(Component::Collider)
    else
    {
      std::ostringstream oss{};
      oss << "Trying to get unsupported component type (" << compType.get_name().to_string() << ") from Entity " << entity.GetEntityID();
      Debug::DebugLogger::GetInstance().LogError(oss.str());
      return rttr::variant();
    }
  }

#define IF_REMOVE_COMP(ComponentClass) if (compType == rttr::type::get<ComponentClass>()) { entity.RemoveComponent<ComponentClass>(); }

  void ObjectFactory::RemoveComponentFromEntity(ECS::Entity entity, rttr::type compType) const
  {
    // get underlying type if it's wrapped in a pointer
    compType = compType.is_wrapper() ? compType.get_wrapped_type().get_raw_type() : compType.is_pointer() ? compType.get_raw_type() : compType;

    IF_REMOVE_COMP(Component::Transform)
    else IF_REMOVE_COMP(Component::Tag)
    else IF_REMOVE_COMP(Component::Layer)
    else IF_REMOVE_COMP(Component::Mesh)
    else IF_REMOVE_COMP(Component::RigidBody)
    else IF_REMOVE_COMP(Component::Collider)
    else
    {
      std::ostringstream oss{};
      oss << "Trying to remove unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::RemoveComponentFromEntity";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
    }
  }

} // namespace Reflection

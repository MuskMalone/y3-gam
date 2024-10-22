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
#include <pch.h>
#include "ObjectFactory.h"
#include "ComponentTypes.h"
#include <Serialization/Deserializer.h>
#include <sstream>
#include <Physics/PhysicsSystem.h>
#include <Prefabs/PrefabManager.h>
#include <Events/EventManager.h>
#include "AddComponentFunctions.h"

#define GET_RTTR_TYPE(T) rttr::type::get<T>()
#ifdef _DEBUG
//#define OF_DEBUG
#endif

namespace Reflection
{

  ObjectFactory::ObjectFactory() {
    using namespace Component;

    mAddComponentFuncs = {
      { GET_RTTR_TYPE(Tag), ComponentUtils::AddTag },
      { GET_RTTR_TYPE(Transform), ComponentUtils::AddTransform },
      { GET_RTTR_TYPE(Layer), ComponentUtils::AddLayer },
      { GET_RTTR_TYPE(Mesh), ComponentUtils::AddMesh },
      { GET_RTTR_TYPE(Material), ComponentUtils::AddMaterial },
      { GET_RTTR_TYPE(BoxCollider), ComponentUtils::AddBoxCollider },
      { GET_RTTR_TYPE(RigidBody), ComponentUtils::AddRigidBody },
      { GET_RTTR_TYPE(ProxyScriptComponent), ComponentUtils::AddScript },
      { GET_RTTR_TYPE(Text), ComponentUtils::AddText }
    };

    if (mAddComponentFuncs.size() != gComponentTypes.size()) {
      throw Debug::Exception<ObjectFactory>(Debug::LVL_CRITICAL,
        Msg("ObjectFactory::mAddComponentFuncs and Reflection::gComponentTypes size mismatch! Did you forget to update one?"));
    }
  }

  void ObjectFactory::AddComponentsToEntity(ECS::Entity id, std::vector<rttr::variant> const& components) const
  {
    for (rttr::variant const& component : components) {
      AddComponentToEntity(id, component);
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

    if (entity.HasComponent<Component::PrefabOverrides>()) {
      newEntity.EmplaceComponent<Component::PrefabOverrides>(entity.GetComponent<Component::PrefabOverrides>());
    }

    // set parent/child
    if (parent) {
      entityMan.SetParentEntity(parent, newEntity);
    }
    if (entityMan.HasChild(entity)) {
      for (ECS::Entity const& child : entityMan.GetChildEntity(entity)) {
        CloneObject(child, newEntity);  // recursively clone all children
      }
    }
  }

  void ObjectFactory::TraverseDownInstance(ECS::Entity base, std::unordered_map<Prefabs::SubDataId, ECS::Entity>& idToEntity,
    ObjectFactory::PrefabInstMap const& prefabInstMap) const
  {
    // if its not in the map, it means this entity was added externally
    if (!prefabInstMap.contains(base.GetRawEnttEntityID())) { return; }

    PrefabInst const& pfbInst{ prefabInstMap.at(base.GetRawEnttEntityID()) };
    ECS::Entity remappedID{ mNewIDs.contains(base.GetRawEnttEntityID()) ? mNewIDs.at(base.GetRawEnttEntityID()) : base };
    // each entity's PrefabOverrides component should 
    // contain an id that corresponds to a sub-object
    idToEntity.emplace(pfbInst.mOverrides.subDataId, remappedID);

    // if no children, return
    if (pfbInst.mChildren.empty()) { return; }

    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
    // else recursively call this function for each child
    for (ECS::Entity e : pfbInst.mChildren) {
      TraverseDownInstance(e, idToEntity, prefabInstMap);
      entityMan.SetParentEntity(remappedID,
        mNewIDs.contains(e.GetRawEnttEntityID()) ? mNewIDs.at(e.GetRawEnttEntityID()) : e);
    }
  }

  void ObjectFactory::OverrideInstanceComponents() const {
    for (ECS::Entity entity : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::PrefabOverrides>()) {
      Component::PrefabOverrides const& overrides{ entity.GetComponent<Component::PrefabOverrides>() };

      // replace any components if needed
      if (!overrides.modifiedComponents.empty()) {
        for (auto const& [type, comp] : overrides.modifiedComponents) {
          AddComponentToEntity(entity, comp);
        }
      }

      // then remove those in the vector
      if (!overrides.removedComponents.empty()) {
        for (rttr::type const& type : overrides.removedComponents) {
          RemoveComponentFromEntity(entity, type);
        }
      }
    }
  }

  void ObjectFactory::LoadPrefabInstances() {
    Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

    for (auto const& [pfb, data] : mPrefabInstances) {
      pm.LoadPrefab(pfb);

      std::vector<ECS::Entity> baseEntities;

      {
        // first, construct each entity while creating a map of parent to children
        // we will use this to traverse down the root entity of each prefab instance,
        for (auto const&[id, instData] : data) {
          ECS::Entity ent{ entityMan.CreateEntityWithID({}, id) };
          // if the ID is taken, map it to the new ID
          if (ent.GetRawEnttEntityID() != id) {
            mNewIDs.emplace(id, ent);
          }

          if (instData.mParent == entt::null) {
            baseEntities.emplace_back(id); // collect the root entities
          }

          // restore its prefab overrides
          ent.EmplaceComponent<Component::PrefabOverrides>(instData.mOverrides);
        }

        auto const& originalPfb{ pm.GetVariantPrefab(pfb) };

        for (ECS::Entity& e : baseEntities) {
          std::unordered_map<Prefabs::SubDataId, ECS::Entity> idToEntity{};
          // traverse down each root entity and
          // create it along with its children
          TraverseDownInstance(e, idToEntity, data);

          // fill the instance with its components and missing sub-objects
          originalPfb.FillPrefabInstance(idToEntity);

        }

        // set the root positions
        for (ECS::Entity& e : baseEntities) {
          std::optional<glm::vec3> const& pos{ data.at(e.GetRawEnttEntityID()).mPosition };
          if (!pos) { continue; }

          // set position if needed
          if (mNewIDs.contains(e.GetRawEnttEntityID())) {
            mNewIDs.at(e.GetRawEnttEntityID()).GetComponent<Component::Transform>().worldPos = *pos;
          }
          else {
            Component::Transform& trans{ e.GetComponent<Component::Transform>() };
            trans.worldPos = trans.position = *pos;
          }
        }
      }
    }

    // override each entity's components
    OverrideInstanceComponents();
  }

  void ObjectFactory::InitScene()
  {
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

    // iterate through data and create entities
    for (auto const& data : mRawEntities)
    {
      ECS::Entity newEntity{ entityMan.CreateEntityWithID({}, data.mID) };

      // if the ID is taken, map it to the new ID
      if (newEntity.GetRawEnttEntityID() != data.mID) {
        mNewIDs.emplace(data.mID, newEntity);
      }

      //entityMan.SetIsActiveEntity(id, arg.mIsActive);
      AddComponentsToEntity(newEntity, data.mComponents);
    }

    // restore the hierarchy
    for (auto const& data : mRawEntities)
    {
      if (data.mParent == entt::null) { continue; }

      // get ID from map if it was re-mapped
      entityMan.SetParentEntity(
        mNewIDs.contains(data.mParent) ? mNewIDs[data.mParent] : data.mParent,
        mNewIDs.contains(data.mID) ? mNewIDs[data.mID] : data.mID);
    }

    LoadPrefabInstances();
  }

  void ObjectFactory::ClearData() {
    mNewIDs.clear();
    mRawEntities.clear();
    mPrefabInstances.clear();
  }

  void ObjectFactory::LoadEntityData(std::string const& filePath) {
    Serialization::Deserializer::DeserializeScene(mRawEntities, mPrefabInstances, filePath);
  }

  void ObjectFactory::AddComponentToEntity(ECS::Entity entity, rttr::variant const& compVar) const
  {
    rttr::type compType{ compVar.get_type() };
    // get underlying type if it's wrapped in a pointer
    compType = compType.is_wrapper() ? compType.get_wrapped_type().get_raw_type() : compType.is_pointer() ? compType.get_raw_type() : compType;

    if (!mAddComponentFuncs.contains(compType)) {
      std::ostringstream oss{};
      oss << "Trying to add unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::AddComponentToEntity";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
      return;
    }

    mAddComponentFuncs.at(compType)(entity, compVar);
  }

  #define IF_GET_ENTITY_COMP(ComponentClass) if (compType == rttr::type::get<ComponentClass>()) {\
    return entity.HasComponent<ComponentClass>() ? std::make_shared<ComponentClass>(entity.GetComponent<ComponentClass>()) : rttr::variant(); }

  rttr::variant ObjectFactory::GetEntityComponent(ECS::Entity const& entity, rttr::type const& compType) const
  {
    IF_GET_ENTITY_COMP(Component::Transform)
    else IF_GET_ENTITY_COMP(Component::Tag)
    else IF_GET_ENTITY_COMP(Component::Layer)
    else IF_GET_ENTITY_COMP(Component::Mesh)
    else IF_GET_ENTITY_COMP(Component::Material)
    else IF_GET_ENTITY_COMP(Component::RigidBody)
    else IF_GET_ENTITY_COMP(Component::BoxCollider)
    else IF_GET_ENTITY_COMP(Component::Script)
    else IF_GET_ENTITY_COMP(Component::Text)
    else
    {
      std::ostringstream oss{};
      oss << "Trying to get unsupported component type (" << compType.get_name().to_string() << ") from Entity " << entity.GetEntityID();
      oss << " | Update ObjectFactory::GetEntityComponent";
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
    else IF_REMOVE_COMP(Component::Material)
    else IF_REMOVE_COMP(Component::RigidBody)
    else IF_REMOVE_COMP(Component::BoxCollider)
    else IF_REMOVE_COMP(Component::Script)
    else IF_REMOVE_COMP(Component::Text)
    else
    {
      std::ostringstream oss{};
      oss << "Trying to remove unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::RemoveComponentFromEntity";
      oss << " | Update ObjectFactory::RemoveComponentFromEntity";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
    }
  }

} // namespace Reflection

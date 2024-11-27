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
#include <Core/Components/Components.h>
#include <Physics/PhysicsSystem.h>

#define GET_RTTR_TYPE(T) rttr::type::get<T>()
#ifdef _DEBUG
//#define OF_DEBUG
#endif

using namespace IGE;

namespace Reflection
{

  ObjectFactory::ObjectFactory() {
    using namespace Component;

    mAddComponentFuncs = {
      { GET_RTTR_TYPE(AudioListener), ComponentUtils::AddAudioListener },      
      { GET_RTTR_TYPE(AudioSource), ComponentUtils::AddAudioSource },
      { GET_RTTR_TYPE(Tag), ComponentUtils::AddTag },
      { GET_RTTR_TYPE(Transform), ComponentUtils::AddTransform },
      { GET_RTTR_TYPE(Layer), ComponentUtils::AddLayer },
      { GET_RTTR_TYPE(Mesh), ComponentUtils::AddMesh },
      { GET_RTTR_TYPE(Material), ComponentUtils::AddMaterial },
      { GET_RTTR_TYPE(BoxCollider), ComponentUtils::AddBoxCollider },
      { GET_RTTR_TYPE(SphereCollider), ComponentUtils::AddSphereCollider },
      { GET_RTTR_TYPE(CapsuleCollider), ComponentUtils::AddCapsuleCollider },
      { GET_RTTR_TYPE(RigidBody), ComponentUtils::AddRigidBody },
      { GET_RTTR_TYPE(ProxyScriptComponent), ComponentUtils::AddScript },
      { GET_RTTR_TYPE(Text), ComponentUtils::AddText },
      { GET_RTTR_TYPE(Light), ComponentUtils::AddLight },
      { GET_RTTR_TYPE(Canvas), ComponentUtils::AddCanvas },
      { GET_RTTR_TYPE(Image), ComponentUtils::AddImage },
      { GET_RTTR_TYPE(Sprite2D), ComponentUtils::AddSprite2D },
      { GET_RTTR_TYPE(Camera), ComponentUtils::AddCamera },
      { GET_RTTR_TYPE(Skybox), ComponentUtils::AddSkybox }
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

  ECS::Entity ObjectFactory::CloneObject(ECS::Entity const& entity, ECS::Entity const& parent) const
  {
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

    ECS::Entity newEntity{ entityMan.CreateEntity() };
    newEntity.SetIsActive(entity.IsActive());

    //std::vector<rttr::variant> const components{ GetEntityComponents(entity) };
    std::vector<rttr::variant> components{};
    components.reserve(3);

    for (rttr::type const& type : Reflection::gComponentTypes) {
      // we're not handling copying of scripts now
      if (type == GET_RTTR_TYPE(Component::Script)) { continue; }

      auto compVar{ GetEntityComponent(entity, type) };
      if (compVar) { components.emplace_back(std::move(compVar)); }
    }

    AddComponentsToEntity(newEntity, components);
    newEntity.GetComponent<Component::Tag>().tag += " (Copy)";

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

    return newEntity;
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

    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
    // in case this instance is nested under an entity, set its parent too
    if (pfbInst.mParent != entt::null) {
      entityMan.SetParentEntity(mNewIDs.contains(pfbInst.mParent) ? mNewIDs.at(pfbInst.mParent) : pfbInst.mParent, remappedID);
    }

    // if no children, return
    if (pfbInst.mChildren.empty()) { return; }

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
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };

    for (auto const& [guid, data] : mPrefabInstances) {
      std::vector<ECS::Entity> baseEntities;

      // first, construct each entity while creating a map of parent to children
      // we will use this to traverse down the root entity of each prefab instance,
      for (auto const& [id, instData] : data) {
        ECS::Entity ent{ entityMan.CreateEntityWithID({}, id) };
        // if the ID is taken, map it to the new ID
        if (ent.GetRawEnttEntityID() != id) {
          mNewIDs.emplace(id, ent);
        }

        if (instData.mParent == entt::null || !data.contains(instData.mParent)) {
          baseEntities.emplace_back(id); // collect the root entities
        }

        // restore its prefab overrides
        ent.EmplaceComponent<Component::PrefabOverrides>(instData.mOverrides);
      }

      try {
        am.LoadRef<Assets::PrefabAsset>(guid);
      }
      // @TODO: Start a blocking call to an ImGui::Popup to allow selection of the prefab file,
      //        then remap the GUIDs to the newly generated one
      catch ([[maybe_unused]] Debug::ExceptionBase& e) {
        PrefabInst const& inst{ data.cbegin()->second };
        IGE_DBGLOGGER.LogCritical("GUID of Prefab: " + inst.mName + " invalid!");
        IGE_DBGLOGGER.LogCritical("Say bye bye to Entity " + ECS::Entity(inst.mId).GetTag() + " until I implement GUI to allow remapping!");
        //IGE_EVENTMGR.DispatchImmediateEvent<Events::RemapPrefabGUID>(inst.mId, inst.mName);
        continue;
      }

      auto const& originalPfb{ am.GetAsset<Assets::PrefabAsset>(guid)->mPrefabData };

      for (ECS::Entity& e : baseEntities) {
        std::unordered_map<Prefabs::SubDataId, ECS::Entity> idToEntity{};
        // traverse down each root entity and
        // create it along with its children
        TraverseDownInstance(e, idToEntity, data);

        // fill the instance with its components and missing sub-objects
        originalPfb.FillPrefabInstance(guid, idToEntity);

      }

      // set the root positions
      for (ECS::Entity& e : baseEntities) {
        std::optional<glm::vec3> const& pos{ data.at(e.GetRawEnttEntityID()).mPosition };
        if (!pos) { continue; }

        // set position if needed
        if (mNewIDs.contains(e.GetRawEnttEntityID())) {
          mNewIDs.at(e.GetRawEnttEntityID()).GetComponent<Component::Transform>().position = *pos;
        }
        else {
          Component::Transform& trans{ e.GetComponent<Component::Transform>() };
          trans.worldPos = trans.position = *pos;
        }
      }
    }

    // override each entity's components
    OverrideInstanceComponents();
    // re-align colliders with entitiies' transforms
    //IGE::Physics::PhysicsSystem::GetInstance()->PausedUpdate();
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

      newEntity.SetIsActive(data.mIsActive);
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
    rttr::type const compType{ compVar.get_type() };
    // get underlying type if it's wrapped in a pointer

    if (!mAddComponentFuncs.contains(compType)) {
      std::ostringstream oss{};
      oss << "Trying to add unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::AddComponentToEntity";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
      return;
    }

    mAddComponentFuncs.at(compType)(entity, compVar);
  }

  #define IF_GET_ENTITY_COMP(ComponentClass) if (compType == rttr::type::get<Component::ComponentClass>()) {\
    return entity.HasComponent<Component::ComponentClass>() ? entity.GetComponent<Component::ComponentClass>() : rttr::variant(); }

  rttr::variant ObjectFactory::GetEntityComponent(ECS::Entity const& entity, rttr::type const& compType) const
  {
    IF_GET_ENTITY_COMP(Transform)
    else IF_GET_ENTITY_COMP(Tag)
    else IF_GET_ENTITY_COMP(Layer)
    else IF_GET_ENTITY_COMP(Mesh)
    else IF_GET_ENTITY_COMP(Material)
    else IF_GET_ENTITY_COMP(RigidBody)
    else IF_GET_ENTITY_COMP(BoxCollider)
    else IF_GET_ENTITY_COMP(SphereCollider)
    else IF_GET_ENTITY_COMP(CapsuleCollider)
    else IF_GET_ENTITY_COMP(Script)
    else IF_GET_ENTITY_COMP(Text)
    else IF_GET_ENTITY_COMP(Light)
    else IF_GET_ENTITY_COMP(AudioListener)
    else IF_GET_ENTITY_COMP(AudioSource)
    else IF_GET_ENTITY_COMP(Canvas)
    else IF_GET_ENTITY_COMP(Image)
    else IF_GET_ENTITY_COMP(Sprite2D)
    else IF_GET_ENTITY_COMP(Camera)
    else IF_GET_ENTITY_COMP(Skybox)
    else
    {
      std::ostringstream oss{};
      oss << "Trying to get unsupported component type (" << compType.get_name().to_string() << ") from Entity " << entity.GetEntityID();
      oss << " | Update ObjectFactory::GetEntityComponent";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
      return rttr::variant();
    }
  }

#define IF_REMOVE_COMP(ComponentClass) if (compType == rttr::type::get<Component::ComponentClass>()) { entity.RemoveComponent<Component::ComponentClass>(); }

  // not in use for now
  void ObjectFactory::RemoveComponentFromEntity(ECS::Entity entity, rttr::type const& compType) const
  {
    UNREFERENCED_PARAMETER(entity); UNREFERENCED_PARAMETER(compType);
    ////// get underlying type if it's wrapped in a pointer

    //IF_REMOVE_COMP(Transform)
    //else IF_REMOVE_COMP(Tag)
    //else IF_REMOVE_COMP(Layer)
    //else IF_REMOVE_COMP(Mesh)
    //else IF_REMOVE_COMP(Material)
    //else IF_REMOVE_COMP(RigidBody)
    //else IF_REMOVE_COMP(BoxCollider)
    //else IF_REMOVE_COMP(SphereCollider)
    //else IF_REMOVE_COMP(CapsuleCollider)
    //else IF_REMOVE_COMP(Script)
    //else IF_REMOVE_COMP(Text)
    //else IF_REMOVE_COMP(Light)
    //else IF_REMOVE_COMP(AudioListener)
    //else IF_REMOVE_COMP(AudioSource)
    //else IF_REMOVE_COMP(Canvas)
    //else IF_REMOVE_COMP(Image)
    //else IF_REMOVE_COMP(Camera)
    //else
    //{
    //  std::ostringstream oss{};
    //  oss << "Trying to remove unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::RemoveComponentFromEntity";
    //  oss << " | Update ObjectFactory::RemoveComponentFromEntity";
    //  Debug::DebugLogger::GetInstance().LogError(oss.str());
    //}
  }

} // namespace Reflection

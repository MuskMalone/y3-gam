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
#include <sstream>
#include <Core/Components/Components.h>
#include "ComponentTypes.h"
#include "ComponentUtils.h"

#include <Serialization/Deserializer.h>
#include <Physics/PhysicsSystem.h>
#include <Prefabs/PrefabManager.h>
#include <Events/EventManager.h>

#define RTTR_TYPE(T) rttr::type::get<T>()
#ifdef _DEBUG
//#define OF_DEBUG
#endif

using namespace IGE;

namespace {
  using AddComponentFunc = std::function<void(ECS::Entity, rttr::variant const&)>;
  using GetComponentFunc = std::function<rttr::variant(ECS::Entity)>;
  using RemoveComponentFunc = std::function<void(ECS::Entity)>;

  using namespace Reflection::ComponentUtils;
  using namespace Component;
#pragma region FunctionMaps
  std::unordered_map<rttr::type, AddComponentFunc> const sAddComponentFuncs{
    { RTTR_TYPE(AudioListener), AddAudioListener },
    { RTTR_TYPE(AudioSource), AddAudioSource },
    { RTTR_TYPE(Bloom), AddBloom },
    { RTTR_TYPE(Tag), AddTag },
    { RTTR_TYPE(Transform), AddTransform },
    { RTTR_TYPE(Layer), AddLayer },
    { RTTR_TYPE(Mesh), AddMesh },
    { RTTR_TYPE(Material), AddMaterial },
    { RTTR_TYPE(BoxCollider), AddBoxCollider },
    { RTTR_TYPE(SphereCollider), AddSphereCollider },
    { RTTR_TYPE(CapsuleCollider), AddCapsuleCollider },
    { RTTR_TYPE(RigidBody), AddRigidBody },
    { RTTR_TYPE(Reflection::ProxyScriptComponent), AddScript },
    { RTTR_TYPE(Text), AddText },
    { RTTR_TYPE(Light), AddLight },
    { RTTR_TYPE(Canvas), AddCanvas },
    { RTTR_TYPE(Image), AddImage },
    { RTTR_TYPE(Sprite2D), AddSprite2D },
    { RTTR_TYPE(Animation), AddAnimation },
    { RTTR_TYPE(Camera), AddCamera },
    { RTTR_TYPE(Skybox), AddSkybox },
    { RTTR_TYPE(Interactive), AddInteractive },
    { RTTR_TYPE(EmitterSystem), AddEmitterSystem },
    { RTTR_TYPE(Video), AddVideo }
  };
  std::unordered_map<rttr::type, GetComponentFunc> const sGetComponentFuncs{
    { RTTR_TYPE(AudioListener), GetComponentVariant<AudioListener> },
    { RTTR_TYPE(AudioSource), GetComponentVariant<AudioSource> },
    { RTTR_TYPE(Bloom), GetComponentVariant<Bloom> },
    { RTTR_TYPE(Tag), GetComponentVariant<Tag> },
    { RTTR_TYPE(Transform), GetComponentVariant<Transform> },
    { RTTR_TYPE(Layer), GetComponentVariant<Layer> },
    { RTTR_TYPE(Mesh), GetComponentVariant<Mesh> },
    { RTTR_TYPE(Material), GetComponentVariant<Material> },
    { RTTR_TYPE(BoxCollider), GetComponentVariant<BoxCollider> },
    { RTTR_TYPE(SphereCollider), GetComponentVariant<SphereCollider> },
    { RTTR_TYPE(CapsuleCollider), GetComponentVariant<CapsuleCollider> },
    { RTTR_TYPE(RigidBody), GetComponentVariant<RigidBody> },
    { RTTR_TYPE(Script), GetComponentVariant<Script> },
    { RTTR_TYPE(Text), GetComponentVariant<Text> },
    { RTTR_TYPE(Light), GetComponentVariant<Light> },
    { RTTR_TYPE(Canvas), GetComponentVariant<Canvas> },
    { RTTR_TYPE(Image), GetComponentVariant<Image> },
    { RTTR_TYPE(Sprite2D), GetComponentVariant<Sprite2D> },
    { RTTR_TYPE(Animation), GetComponentVariant<Animation> },
    { RTTR_TYPE(Camera), GetComponentVariant<Camera> },
    { RTTR_TYPE(Skybox), GetComponentVariant<Skybox> },
    { RTTR_TYPE(Interactive), GetComponentVariant<Interactive> },
    { RTTR_TYPE(EmitterSystem), GetComponentVariant<EmitterSystem> },
    { RTTR_TYPE(Video), GetComponentVariant<Video> }
  };
  std::unordered_map<rttr::type, RemoveComponentFunc> const sRemoveComponentFuncs{
    { RTTR_TYPE(AudioListener), RemoveComponent<AudioListener> },
    { RTTR_TYPE(AudioSource), RemoveComponent<AudioSource> },
    { RTTR_TYPE(Bloom), RemoveComponent<Bloom> },
    { RTTR_TYPE(Tag), RemoveComponent<Tag> },
    { RTTR_TYPE(Transform), RemoveComponent<Transform> },
    { RTTR_TYPE(Layer), RemoveComponent<Layer> },
    { RTTR_TYPE(Mesh), RemoveComponent<Mesh> },
    { RTTR_TYPE(Material), RemoveComponent<Material> },
    { RTTR_TYPE(BoxCollider), RemoveComponent<BoxCollider> },
    { RTTR_TYPE(SphereCollider), RemoveComponent<SphereCollider> },
    { RTTR_TYPE(CapsuleCollider), RemoveComponent<CapsuleCollider> },
    { RTTR_TYPE(RigidBody), RemoveComponent<RigidBody> },
    { RTTR_TYPE(Script), RemoveComponent<Script> },
    { RTTR_TYPE(Text), RemoveComponent<Text> },
    { RTTR_TYPE(Light), RemoveComponent<Light> },
    { RTTR_TYPE(Canvas), RemoveComponent<Canvas> },
    { RTTR_TYPE(Image), RemoveComponent<Image> },
    { RTTR_TYPE(Sprite2D), RemoveComponent<Sprite2D> },
    { RTTR_TYPE(Animation), RemoveComponent<Animation> },
    { RTTR_TYPE(Camera), RemoveComponent<Camera> },
    { RTTR_TYPE(Skybox), RemoveComponent<Skybox> },
    { RTTR_TYPE(Interactive), RemoveComponent<Interactive> },
    { RTTR_TYPE(EmitterSystem), RemoveComponent<EmitterSystem> },
    { RTTR_TYPE(Video), RemoveComponent<Video> }
  };
#pragma endregion
}

namespace Reflection
{
  ObjectFactory::ObjectFactory() {
    using namespace Component;

    if (sAddComponentFuncs.size() != gComponentTypes.size()) {
#ifndef DISTRIBUTION
      std::cout << "ObjectFactory::mAddComponentFuncs and Reflection::gComponentTypes size mismatch! Did you forget to update one?\n";
#endif
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
      if (type == RTTR_TYPE(Component::Script)) { continue; }

      auto compVar{ GetEntityComponent(entity, type) };
      if (compVar) { components.emplace_back(std::move(compVar)); }
    }

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
    for (auto const&[guid, pfbInstMap] : mPrefabInstances) {
      for (auto const& [id, pfbInstData] : pfbInstMap) {
        Serialization::PfbOverridesData const& overrides{ pfbInstData.mOverrides };
        ECS::Entity entity{ mNewIDs.contains(id) ? mNewIDs.at(id) : id };

        // replace any components if needed
        if (!overrides.componentData.empty()) {
          for (auto const& [type, comp] : overrides.componentData) {
            AddComponentToEntity(entity, comp);
          }
        }

        // then remove those in the vector
        if (!overrides.removedComponents.empty()) {
          for (std::string const& typeStr : overrides.removedComponents) {
            RemoveComponentFromEntity(entity, rttr::type::get_by_name(typeStr));
          }
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
      bool isPrefabValid{ true };

      try {
        am.LoadRef<Assets::PrefabAsset>(guid);
      }
      catch ([[maybe_unused]] Debug::ExceptionBase& e) {
        // if invalid, dispatch event to remap the reference
        PrefabInst const& inst{ data.cbegin()->second };
        IGE_DBGLOGGER.LogCritical("GUID: " + std::to_string(static_cast<uint64_t>(guid)) + " of Prefab: " + inst.mName + " invalid!");
        QUEUE_EVENT(Events::GUIDInvalidated, data.begin()->first, guid, rttr::type::get<IGE::Assets::PrefabAsset>().get_name().to_string());

        // we'll continue to create the entity, just without the prefab overrides
        isPrefabValid = false;
      }

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
        if (isPrefabValid) {
          if (ent.GetEntityID() == 5243086) {
            std::cout << "Restoring entity " << ent.GetTag() << " " << ent.GetEntityID() << "\n";
          }
          ent.EmplaceComponent<Component::PrefabOverrides>(instData.mOverrides.ToPrefabOverrides());
        }
      }

      Prefabs::Prefab const* originalPfb{ isPrefabValid ? &am.GetAsset<Assets::PrefabAsset>(guid)->mPrefabData : nullptr };

      for (ECS::Entity& e : baseEntities) {
        std::unordered_map<Prefabs::SubDataId, ECS::Entity> idToEntity{};
        // traverse down each root entity and
        // create it along with its children
        TraverseDownInstance(e, idToEntity, data);

        // fill the instance with its components and missing sub-objects
        if (isPrefabValid) {
          originalPfb->FillPrefabInstance(guid, idToEntity);
        }
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
  }

  void ObjectFactory::InitScene()
  {
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

#ifndef DISTRIBUTION
    try {
#endif
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
#ifndef DISTRIBUTION
    }
    catch (Debug::ExceptionBase& e) {
      e.LogSource();
    }
#endif

    // trigger the guid remapping popup if needed
    QUEUE_EVENT(Events::TriggerGUIDRemap);
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

#ifdef _DEBUG
    if (!sAddComponentFuncs.contains(compType)) {
      std::ostringstream oss{};
      oss << "Trying to add unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::AddComponentToEntity";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
      return;
    }
#endif

    sAddComponentFuncs.at(compType)(entity, compVar);
  }

  rttr::variant ObjectFactory::GetEntityComponent(ECS::Entity const& entity, rttr::type const& compType) const
  {
#ifdef _DEBUG
    if (!sGetComponentFuncs.contains(compType)) {
      std::ostringstream oss{};
      oss << "Trying to get unsupported component type (" << compType.get_name().to_string() << ") from Entity " << entity.GetEntityID();
      oss << " | Update ObjectFactory::GetEntityComponent";
      Debug::DebugLogger::GetInstance().LogError(oss.str());
      return rttr::variant();
    }
#endif

    return sGetComponentFuncs.at(compType)(entity);
  }

  // not in use for now
  void ObjectFactory::RemoveComponentFromEntity(ECS::Entity entity, rttr::type const& compType) const
  {
#ifdef _DEBUG
    if (!sRemoveComponentFuncs.contains(compType)) {
        std::ostringstream oss{};
        oss << "Trying to remove unknown component type: " << compType.get_name().to_string() << " to entity " << entity << " | Update ObjectFactory::RemoveComponentFromEntity";
        oss << " | Update ObjectFactory::RemoveComponentFromEntity";
        Debug::DebugLogger::GetInstance().LogError(oss.str());
    }
#endif

    sRemoveComponentFuncs.at(compType)(entity);
  }

} // namespace Reflection

/*!*********************************************************************
\file   AddComponentFunctions.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Contains the definition of functions used by the ObjectFactory
        to add each component to an entity. This is so that each
        component can be added their own way.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "AddComponentFunctions.h"
#include <Physics/PhysicsSystem.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>
#include <Asset/IGEAssets.h>

namespace Reflection::ComponentUtils {
  using namespace Component;

#define EXTRACT_RAW_COMP(T, variable) T const& variable{ var.get_type().is_wrapper() ? var.get_wrapped_value<T>() : var.get_value<T>() }

  void AddTag(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Tag, comp);

    entity.EmplaceOrReplaceComponent<Tag>(comp);
  }

  void AddTransform(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Transform, comp);

    entity.EmplaceOrReplaceComponent<Transform>(comp);
  }

  void AddLayer(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Layer, comp);

    entity.EmplaceOrReplaceComponent<Layer>(comp);
  }

  void AddBoxCollider(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(BoxCollider, comp);

    //entity.EmplaceOrReplaceComponent<Collider>(comp);
    IGE::Physics::PhysicsSystem::GetInstance()->AddBoxCollider(entity, comp);
  }

  void AddSphereCollider(ECS::Entity entity, rttr::variant const& var){
      EXTRACT_RAW_COMP(SphereCollider, comp);

      //entity.EmplaceOrReplaceComponent<Collider>(comp);
      IGE::Physics::PhysicsSystem::GetInstance()->AddSphereCollider(entity, comp);
  }

  void AddCapsuleCollider(ECS::Entity entity, rttr::variant const& var){
      EXTRACT_RAW_COMP(CapsuleCollider, comp);

      //entity.EmplaceOrReplaceComponent<Collider>(comp);
      IGE::Physics::PhysicsSystem::GetInstance()->AddCapsuleCollider(entity, comp);
  }

  void AddRigidBody(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(RigidBody, comp);

    //entity.EmplaceOrReplaceComponent<RigidBody>(comp);
    IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(entity, comp);
  }

  void AddMaterial(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Material, comp);

    entity.EmplaceOrReplaceComponent<Material>();
  }

  void AddMesh(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Mesh, comp);

    IGE::Assets::GUID const& meshSrc{ IGE_ASSETMGR.LoadRef<IGE::Assets::MeshAsset>(comp.meshName) };//Graphics::MeshFactory::CreateModelFromString(comp.meshName) };
    entity.EmplaceOrReplaceComponent<Mesh>(meshSrc, comp.meshName);
  }

  void AddText(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Text, comp);

    entity.EmplaceOrReplaceComponent<Text>(comp);
  }

  void AddScript(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(ProxyScriptComponent, comp);

    entity.EmplaceOrReplaceComponent<Script>(comp);
  }

  void AddLight(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Light, comp);

    entity.EmplaceOrReplaceComponent<Light>(comp);
  }

} // namespace Reflection

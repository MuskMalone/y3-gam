#include <pch.h>
#include "AddComponentFunctions.h"
#include <Physics/PhysicsSystem.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>

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

  void AddCollider(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Collider, comp);

    //entity.EmplaceOrReplaceComponent<Collider>(comp);
    IGE::Physics::PhysicsSystem::GetInstance()->AddCollider(entity);
  }

  void AddRigidBody(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(RigidBody, comp);

    //entity.EmplaceOrReplaceComponent<RigidBody>(comp);
    IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(entity);
  }

  void AddMaterial(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Material, comp);


  }

  void AddMesh(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Mesh, comp);

    auto meshSrc{ Graphics::MeshFactory::CreateModelFromString(comp.meshName) };
    entity.EmplaceOrReplaceComponent<Mesh>(std::make_shared<Graphics::Mesh>(meshSrc), comp.meshName);
  }

  void AddText(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Text, comp);

    entity.EmplaceOrReplaceComponent<Text>(comp);
  }

  void AddScript(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Script, comp);

    //entity.EmplaceOrReplaceComponent<Script>(comp);
  }

} // namespace Reflection

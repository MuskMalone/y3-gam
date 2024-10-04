#pragma once
#include <Core/Components/Components.h>
#include <Core/Entity.h>
#include <rttr/variant.h>

namespace Reflection::ComponentUtils {

  void AddTag(ECS::Entity entity, rttr::variant const& var);
  void AddTransform(ECS::Entity entity, rttr::variant const& var);
  void AddLayer(ECS::Entity entity, rttr::variant const& var);
  void AddMesh(ECS::Entity entity, rttr::variant const& var);
  void AddMaterial(ECS::Entity entity, rttr::variant const& var);
  void AddCollider(ECS::Entity entity, rttr::variant const& var);
  void AddRigidBody(ECS::Entity entity, rttr::variant const& var);
  void AddText(ECS::Entity entity, rttr::variant const& var);
  void AddScript(ECS::Entity entity, rttr::variant const& var);

} // namespace Reflection

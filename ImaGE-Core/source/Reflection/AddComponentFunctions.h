/*!*********************************************************************
\file   AddComponentFunctions.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Contains the declaration of functions used by the ObjectFactory
        to add each component to an entity. This is so that each
        component can be added their own way.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/Components/Components.h>
#include <Core/Entity.h>
#include <rttr/variant.h>

namespace Reflection::ComponentUtils {
  void AddAudioListener(ECS::Entity entity, rttr::variant const& var);  
  void AddAudioSource(ECS::Entity entity, rttr::variant const& var);
  void AddTag(ECS::Entity entity, rttr::variant const& var);
  void AddTransform(ECS::Entity entity, rttr::variant const& var);
  void AddLayer(ECS::Entity entity, rttr::variant const& var);
  void AddMesh(ECS::Entity entity, rttr::variant const& var);
  void AddMaterial(ECS::Entity entity, rttr::variant const& var);
  void AddBoxCollider(ECS::Entity entity, rttr::variant const& var);
  void AddSphereCollider(ECS::Entity entity, rttr::variant const& var);
  void AddCapsuleCollider(ECS::Entity entity, rttr::variant const& var);
  void AddRigidBody(ECS::Entity entity, rttr::variant const& var);
  void AddText(ECS::Entity entity, rttr::variant const& var);
  void AddScript(ECS::Entity entity, rttr::variant const& var);
  void AddLight(ECS::Entity entity, rttr::variant const& var);
  void AddCanvas(ECS::Entity entity, rttr::variant const& var);
  void AddImage(ECS::Entity entity, rttr::variant const& var);
  void AddSprite2D(ECS::Entity entity, rttr::variant const& var);
  void AddCamera(ECS::Entity entity, rttr::variant const& var);
} // namespace Reflection

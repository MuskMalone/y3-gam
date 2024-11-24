/*!*********************************************************************
\file   Transform.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of the Transform component. Contains local and
        world values for an entity, where local values are relative to
        its parent.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Transform.h"
#include <Core/Systems/TransformSystem/TransformHelpers.h>

namespace Component {

  glm::mat4 Transform::GetLocalMdlMatrix() const {
    glm::mat4 t{ glm::translate(glm::mat4{ 1.f }, position) };
    glm::mat4 r{ glm::toMat4(rotation) };
    glm::mat4 s{ glm::scale(glm::mat4{ 1.f }, scale) };

    return t * r * s;
  }

  void Transform::SetLocalRotWithEuler(glm::vec3 const& degrees) {
    eulerAngles = degrees;
    rotation = glm::quat(glm::radians(degrees));
  }

  void Transform::ApplyWorldRotation(float degrees, glm::vec3 const& axis) {
    worldRot = glm::angleAxis(glm::radians(degrees), axis) * worldRot;
    modified = true;
  }

  glm::vec3 Transform::GetWorldEulerAngles() const {
    return glm::degrees(glm::eulerAngles(worldRot));
  }

  void Transform::SetLocalToWorld() noexcept {
    position = worldPos;
    scale = worldScale;
    rotation = worldRot;
    eulerAngles = glm::degrees(glm::eulerAngles(rotation));
  }

  void Transform::ComputeWorldMtx() {
    glm::mat4 trans{ glm::translate(glm::mat4{ 1.f }, worldPos) };
    glm::mat4 rot{ glm::toMat4(worldRot) };
    glm::mat4 scale{ glm::scale(glm::mat4{ 1.f }, worldScale) };

    worldMtx = trans * rot * scale;
  }

  void Transform::ResetLocal() {
    position = eulerAngles = {};
    rotation = { 1.f, 0.f, 0.f, 0.f };
    scale = { 1.f, 1.f, 1.f };
  }

  void Transform::Clear() noexcept {
    position = worldPos = eulerAngles = {};
    scale = worldScale = { 1.f, 1.f, 1.f };
    rotation = worldRot = { 1.f, 0.f, 0.f, 0.f };
    worldMtx = parentWorldMtx = glm::identity<glm::mat4>();
    modified = true;
  }

} // namespace Component

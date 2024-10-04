#include <pch.h>
#include "Transform.h"

namespace Component {

  void Transform::SetLocalRotWithEuler(glm::vec3 const& degrees) {
    eulerAngles = degrees;
    rotation = glm::quat(glm::radians(degrees));
  }

  glm::vec3 Transform::GetWorldEulerAngles() const {
    return glm::degrees(glm::eulerAngles(worldRot));
  }

  void Transform::SetLocalToWorld() noexcept {
    position = worldPos;
    scale = worldScale;
    rotation = worldRot;
  }

  void Transform::ComputeWorldMtx() {
    glm::mat4 trans{ glm::translate(glm::mat4{ 1.f }, worldPos) };
    glm::mat4 rot{ glm::toMat4(worldRot) };
    glm::mat4 scale{ glm::scale(glm::mat4{ 1.f }, worldScale) };

    worldMtx = trans * rot * scale;
  }

  void Transform::Clear() noexcept {
    position = eulerAngles = {};
    scale = { 1.f, 1.f, 1.f };
    worldPos = worldScale = {};
    rotation = worldRot = { 1.f, 0.f, 0.f, 0.f };
    worldMtx = parentWorldMtx = glm::identity<glm::mat4>();
    modified = true;
  }

} // namespace Component

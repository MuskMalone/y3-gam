#pragma once
#include <glm/glm.hpp>

namespace Component
{

  struct Transform
  {
    Transform() = default;
    Transform(glm::vec3 const& _pos, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {})
      : localPos{}, localScale{}, localRot{}, worldPos{ _pos }, worldScale{ _scale }, worldRot{ _rot } {}

    inline void Clear() noexcept {
      localPos = glm::vec3(0);
      localScale = glm::vec3(0);
      localRot = glm::vec3(0);

      worldPos = glm::vec3(0);
      worldScale = glm::vec3(0);
      worldRot = glm::vec3(0);
    }

    glm::vec3 localPos, localScale, localRot;

    glm::vec3 worldPos, worldScale, worldRot;
    //glm::quat rot; //change rot to quat
  };

}

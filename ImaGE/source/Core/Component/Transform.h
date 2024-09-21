#pragma once
#include <glm/glm.hpp>

namespace Component
{

  struct Transform
  {
    Transform() = default;
    Transform(glm::dvec3 const& _pos, glm::dvec3 const& _scale = { 1.f, 1.f, 1.f }, glm::dvec3 const& _rot = {})
      : localPos{}, localScale{}, localRot{}, worldPos{ _pos }, worldScale{ _scale }, worldRot{ _rot } {}

    inline void Clear() noexcept {
      localPos = glm::dvec3(0);
      localScale = glm::dvec3(0);
      localRot = glm::dvec3(0);

      worldPos = glm::dvec3(0);
      worldScale = glm::dvec3(0);
      worldRot = glm::dvec3(0);
    }

    glm::dvec3 localPos, localScale, localRot;

    glm::dvec3 worldPos, worldScale, worldRot;
    //glm::quat rot; //change rot to quat
  };

}

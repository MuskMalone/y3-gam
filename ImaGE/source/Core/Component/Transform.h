#pragma once
#include <glm/glm.hpp>

namespace Component
{

  struct Transform
  {
    Transform(glm::vec3 const& _pos = {}, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {})
      : localPos{}, localScale{}, localRot{}, worldPos{ _pos }, worldScale{ _scale }, worldRot{ _rot } {}

    glm::vec3 localPos, localScale, localRot;
    glm::vec3 worldPos, worldScale, worldRot;
  };

}

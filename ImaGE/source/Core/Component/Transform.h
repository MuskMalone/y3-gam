#pragma once
#include <glm/glm.hpp>

namespace Component
{

  struct Transform
  {
    Transform(glm::vec3 const& _pos = {}, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {})
      : position{ _pos }, scale{ _scale }, rotation{ _rot } {}

    glm::vec3 position, scale, rotation;
  };

}

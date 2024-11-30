#pragma once
#include <glm/glm.hpp>

namespace BV {
  struct AABB {
    AABB() = default;
    AABB(glm::vec3 const& _center, glm::vec3 _halfExtents) : center{ _center }, halfExtents{ _halfExtents } {}

    glm::vec3 center, halfExtents;

    glm::vec3 GetMin() const { return center - halfExtents; }
    glm::vec3 GetMax() const { return center + halfExtents; }
  };
}
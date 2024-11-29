#pragma once
#include <glm/glm.hpp>

namespace BV {
  struct BoundingSphere {
    BoundingSphere() = default;
    BoundingSphere(glm::vec3 const& _center, float _radius) : center{ _center }, radius{ _radius } {}

    glm::vec3 center;
    float radius;
  };
}

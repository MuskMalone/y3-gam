#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Intersection
{

  struct Result
  {
    bool collided;

    Result(bool _collided) : collided{ _collided } {}
    inline operator bool() const noexcept { return collided; }
  };


  struct PointResult : Result
  {
    glm::vec3 interPt;

    PointResult(bool collided) : interPt{}, Result(collided) {}
    PointResult(glm::vec3 const& interPoint, bool collided = true) : interPt{ interPoint }, Result(collided) {}
  };

} // namespace Intersection

#include <pch.h>
#include "Primitives.h"
#include <Graphics/MeshGen.h>

namespace Primitives
{

  glm::vec3 Triangle::GetBarycentricCoords(glm::vec3 const& point) const
  {
    glm::vec3 ret;
    glm::vec3 const e0{ points[1] - points[0] }, e1{ points[2] - points[0] }, e2{ point - points[0] };
    float const d00{ glm::dot(e0, e0) }, d01{ glm::dot(e0, e1) },
      d11{ glm::dot(e1, e1) }, d20{ glm::dot(e2, e0) }, d21{ glm::dot(e2, e1) };
    float const invDenom{ 1.f / (d00 * d11 - d01 * d01) };

    ret.x = invDenom * (d11 * d20 - d01 * d21);
    ret.y = invDenom * (d00 * d21 - d01 * d20);
    ret.z = 1.f - ret.x - ret.y;
    return ret;
  }

  Plane::Plane(Triangle const& triangle)
  {
    glm::vec3 const norm{ glm::normalize(glm::cross(triangle[1] - triangle[0], triangle[2] - triangle[0])) };
    normal = glm::vec4(norm, -glm::dot(norm, triangle[0]));
  }

  float Plane::GetPointDist(glm::vec3 const& point) const
  {
    return glm::dot(glm::vec3(normal), point) + normal.w;
  }

} // namespace Primitives

#pragma once
#include "Result.h"
#include "Primitives.h"

// forward declarations
namespace Primitives { struct Point3D; struct Plane; struct Triangle; struct Ray; }
namespace BV { struct BoundingSphere; struct AABB; }

namespace Intersection
{
  
  Result SphereToSphere(BV::BoundingSphere const& s1, BV::BoundingSphere const& s2);
  Result AABBToSphere(BV::AABB const& rect, BV::BoundingSphere const& sphere);
  Result AAABToAABB(BV::AABB const& rect1, BV::AABB const& rect2);

  Result PointInSphere(glm::vec3 const& point, BV::BoundingSphere const& sphere);
  Result PointInAABB(glm::vec3 const& point, BV::AABB const& rect);

  Result PointInTriangle(glm::vec3 const& point, Primitives::Triangle const& triangle);
  Result PointInPlane(glm::vec3 const& point, Primitives::Plane const& plane);

  Result RayToTriangle(Primitives::Ray const& ray, Primitives::Triangle const& triangle);
  PointResult RayToPlane(Primitives::Ray const& ray, Primitives::Plane const& plane);
  PointResult RayToAABB(Primitives::Ray const& ray, BV::AABB const& rect);
  Result RayToSphere(Primitives::Ray const& ray, BV::BoundingSphere const& sphere);

  Result PlaneToAABB(Primitives::Plane const& plane, BV::AABB const& rect);
  Result PlaneToSphere(Primitives::Plane const& plane, BV::BoundingSphere const& sphere);

  glm::vec3 LineSegmentToPlane(const glm::vec3& start, const glm::vec3& end, Primitives::Plane plane);

} // namespace Intersection

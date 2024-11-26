#pragma once
#include <BoundingVolumes/AABB.h>
#include <BoundingVolumes/BoundingSphere.h>
#include <BoundingVolumes/Frustum.h>

namespace BV {
  bool PlaneSphereIntersection(Plane const& plane, BoundingSphere const& sphere);
  bool PlaneAABBIntersection(Plane const& plane, AABB const& aabb);
  bool FrustumSphereIntersection(Frustum const& frustum, BoundingSphere const& sphere);
  bool FrustumAABBIntersection(Frustum const& frustum, AABB const& aabb);
}

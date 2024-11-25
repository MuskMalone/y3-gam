#include <pch.h>
#include "IntersectionTests.h"

namespace BV {
  bool PlaneSphereIntersection(Plane const& plane, BoundingSphere const& sphere) {
    return plane.GetPlaneSignedDist(sphere.center) > -sphere.radius;
  }

  bool PlaneAABBIntersection(Plane const& plane, AABB const& aabb) {
    float const r{ aabb.halfExtents.x * std::abs(plane.normal.x)
      + aabb.halfExtents.y * std::abs(plane.normal.y)
      + aabb.halfExtents.z * std::abs(plane.normal.z) };

    return -r - glm::epsilon<float>() <= plane.GetPlaneSignedDist(aabb.center);
  }

  bool FrustumSphereIntersection(Frustum const& frustum, BoundingSphere const& sphere) {
    for (int i{}; i < 6; ++i) {
      if (frustum[i].normal.x * sphere.center.x + frustum[i].normal.y * sphere.center.y + frustum[i].normal.z * sphere.center.z + frustum[i].dist <= -sphere.radius) {
        return false;
      }
    }

    return true;
  }

  bool FrustumAABBIntersection(Frustum const& frustum, AABB const& aabb) {
    for (int i{}; i < 6; ++i) {
      if (!PlaneAABBIntersection(frustum[i], aabb)) {
        return false;
      }
    }

    return true;
  }
}

#include <pch.h>
#include "IntersectionTests.h"

namespace BV {
  bool PlaneSphereIntersection(Plane const& plane, BoundingSphere const& sphere) {
    return plane.GetPlaneSignedDist(sphere.center) > -sphere.radius - glm::epsilon<float>();
  }

  bool PlaneAABBIntersection(Plane const& plane, AABB const& aabb) {
    glm::vec3 const extents{ aabb.halfExtents * 2.f };

    float const r{ extents.x * std::abs(plane.normal.x)
      + extents.y * std::abs(plane.normal.y)
      + extents.z * std::abs(plane.normal.z) };

    return -r - glm::epsilon<float>() <= plane.GetPlaneSignedDist(aabb.center);
  }

  bool FrustumSphereIntersection(Frustum const& frustum, BoundingSphere const& sphere) {
    for (int i{}; i < 6; ++i) {
      if (!PlaneSphereIntersection(frustum[i], sphere)) {
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

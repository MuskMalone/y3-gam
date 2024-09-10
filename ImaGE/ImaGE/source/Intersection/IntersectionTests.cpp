#include <pch.h>
#include "IntersectionTests.h"
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <BoundingVolumes/BoundingSphere.h>
#include <BoundingVolumes/AABB.h>

using namespace Primitives;
using namespace BV;

namespace
{
  bool CheckEqualsEpsilon(float val, float comparison = 0.f);
  glm::vec3 GetClosestPoint(glm::vec3 const& point, BV::AABB const& aabb);
} // anonymous namespace


// functions
namespace Intersection
{

  Result SphereToSphere(BoundingSphere const& s1, BoundingSphere const& s2)
  {
    return PointInSphere(s1.center, { s2.center, s1.radius + s2.radius });
  }


  Result AABBToSphere(AABB const& rect, BoundingSphere const& sphere)
  {
    float const distSquared{ glm::distance2(GetClosestPoint(sphere.center, rect), sphere.center) };

    return distSquared <= sphere.radius * sphere.radius + EPSILON<float>;
  }

  Result AAABToAABB(AABB const& rect1, AABB const& rect2)
  {
    glm::vec3 const r1Min{ rect1.GetMin() }, r1Max{ rect1.GetMax() },
      r2Min{ rect2.GetMin() }, r2Max{ rect2.GetMax() };

    for (unsigned i{}; i < 3; ++i)
    {
      if (r1Max[i] < r2Min[i] || r2Max[i] < r1Min[i]) {
        return false;
      }
    }

    return true;
  }

  Result PointInSphere(glm::vec3 const& point, BoundingSphere const& sphere)
  {
    float const distSquared{ glm::distance2(point, sphere.center) };

    return distSquared <= sphere.radius * sphere.radius + EPSILON<float>;
  }

  Result PointInAABB(glm::vec3 const& point, AABB const& rect)
  {
    glm::vec3 const min{ rect.GetMin() }, max{ rect.GetMax() };

    for (int i{}; i < 3; ++i) {
      if (point[i] < min[i] || point[i] > max[i]) { return false; }
    }
    return true;
  }

  Result PointInTriangle(glm::vec3 const& point, Triangle const& triangle)
  {
    glm::vec3 const barycentric{ triangle.GetBarycentricCoords(point) };

    return barycentric.x >= 0.f && barycentric.y >= 0.f && barycentric.x + barycentric.y <= 1.f;
  }



  Result PointInPlane(Point3D const& point, Plane const& plane)
  {
    return CheckEqualsEpsilon(glm::dot(point.coordinates, glm::vec3(plane.normal)), plane.normal.w);
  }


  Result RayToTriangle(Ray const& ray, Triangle const& triangle)
  {
    glm::vec3 const normal{ glm::cross(triangle.points[1] - triangle.points[0], triangle.points[2] - triangle.points[0])};
    float const t{ glm::dot(-normal, ray.start - triangle.points[0]) / glm::dot(normal, ray.direction)};

    if (t < 0.f) { return false; }

    return PointInTriangle(ray.start + glm::normalize(ray.direction) * t, triangle);
  }

  PointResult RayToPlane(Ray const& ray, Plane const& plane)
  {
    glm::vec3 const vec3Normal{ glm::vec3(plane.normal) };
    if (CheckEqualsEpsilon(glm::dot(ray.direction, vec3Normal)))
    {
      float const t{ (plane.normal.w - glm::dot(vec3Normal, ray.start)) / glm::dot(vec3Normal, glm::normalize(ray.direction)) };
      return t >= 0.f && t <= 1.f ? PointResult(ray.start + t * ray.direction) : PointResult(false);
    }

    return false;
  }

  PointResult RayToAABB(Ray const& ray, AABB const& rect)
  {
    float tMin{}, tMax{ FLT_MAX };
    glm::vec3 const min{ rect.GetMin() }, max{ rect.GetMax() };

    for (unsigned i{}; i < 3; ++i)
    {
      if (abs(ray.direction[i]) < EPSILON<float>)
      {
        if (ray.start[i] < min[i] || ray.start[i] > max[i]) {
          return false;
        }
      }
      else
      {
        float const ood = 1.f / ray.direction[i];
        float t1{ (min[i] - ray.start[i]) * ood };
        float t2{ (max[i] - ray.start[i]) * ood };

        // ensure t1 < t2
        if (t1 > t2) { std::swap(t1, t2); }

        tMin = glm::max(tMin, t1);
        tMax = glm::min(tMax, t2);

        if (tMin > tMax) { return false; }
      }
    }

    return { ray.start + tMin * ray.direction };
  }

  Result RayToSphere(Ray const& ray, BoundingSphere const& sphere)
  {
    glm::vec3 eyeToSphere{ ray.start - sphere.center };
    float const c{ glm::dot(eyeToSphere, eyeToSphere) - sphere.radius * sphere.radius };

    if (c <= 0.f) { return true; }

    float const b{ 2.f * glm::dot(eyeToSphere, ray.direction) };
    if (b > 0.f) { return false; }

    return !(b * b - 4.f * glm::dot(ray.direction, ray.direction) * c < 0.f);
  }


  Result PlaneToAABB(Plane const& plane, AABB const& rect)
  {
    glm::vec3 const min{ rect.GetMin() }, max{ rect.GetMax() };
    float const dist1{ plane.GetPointDist(min) }, dist2{ plane.GetPointDist(max) };

    return dist1 * dist2 < 0.f;
  }

  Result PlaneToSphere(Plane const& plane, BoundingSphere const& sphere)
  {
    return abs(plane.GetPointDist(sphere.center)) <= sphere.radius;
  }

  glm::vec3 LineSegmentToPlane(const glm::vec3& start, const glm::vec3& end, Primitives::Plane plane)
  {
    glm::vec3 direction = end - start;
    float t = -plane.GetPointDist(start) / glm::dot(glm::vec3(plane.normal), direction);
    glm::vec3 intersection = start + t * direction;
    return intersection;
  }

} // namespace Intersection


namespace
{
  bool CheckEqualsEpsilon(float val, float comparison)
  {
    return abs(val - comparison) <= EPSILON<float>;
  }

  glm::vec3 GetClosestPoint(glm::vec3 const& point, BV::AABB const& aabb)
  {
    glm::vec3 ret;
    glm::vec3 const& min{ aabb.GetMin() }, &max{ aabb.GetMax() };

    for (unsigned i{}; i < 3; ++i)
    {
      ret[i] = glm::clamp(point[i], min[i], max[i]);
    }

    return ret;
  }
} // anonymous namespace
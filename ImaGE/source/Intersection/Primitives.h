#pragma once
#include <glm/glm.hpp>

namespace Primitives
{

  struct Point3D
  {
    Point3D(glm::vec3 const& pt) : coordinates{ pt } {}
    inline operator glm::vec3 const& () const { return coordinates; }

    // (x, y, z)
    glm::vec3 coordinates;
  };

  struct Triangle
  {
    Triangle(glm::vec3 const& pt1, glm::vec3 const& pt2, glm::vec3 const& pt3)
    {
      points[0] = pt1; points[1] = pt2; points[2] = pt3;
    }
    
    glm::vec3 GetBarycentricCoords(glm::vec3 const& point) const;

    inline glm::vec3& operator[](unsigned idx) { return points[idx]; }
    inline glm::vec3 const& operator[](unsigned idx) const { return points[idx]; }

    glm::vec3 points[3];
  };

  struct Plane
  {
    Plane() = default;
    Plane(glm::vec4 const& _normal) : normal{ glm::normalize(_normal) } {}
    Plane(Triangle const& triangle);

    float GetPointDist(glm::vec3 const& point) const;

    // (n.x, n.y, n.z, d)
    glm::vec4 normal;
  };

  // ray = start + dir
  struct Ray
  {
    Ray(glm::vec3 const& p0, glm::vec3 const& p1) : start{ p0 }, direction{ p1 - p0 } {}

    glm::vec3 start;
    glm::vec3 direction;
  };

} //  namespace Primitives

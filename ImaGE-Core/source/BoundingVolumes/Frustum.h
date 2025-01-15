#pragma once
#include <glm/glm.hpp>

namespace BV {

  struct Plane {
    Plane() = default;
    Plane(glm::vec3 const& _normal, float _dist) : normal{ _normal }, dist{ _dist } {}
    Plane(glm::vec3 const& point, glm::vec3 const& _normal) : normal{ glm::normalize(_normal) }, dist{ glm::dot(point, normal) } {}
    Plane(float a, float b, float c, float d) : normal{ a, b, c }, dist{ d } {}

    float GetPlaneSignedDist(glm::vec3 const& point) const {
      return glm::dot(normal, point) + dist;
    }
      
    void Normalize() {
      float const mag{ glm::length(normal) };
      normal /= mag;
      dist /= mag;
    }

    glm::vec3 normal;
    float dist;
  };

  struct Frustum {
    union {
      Plane planes[6];

      // accessors for individual planes
      struct {
        Plane leftP, rightP, topP,
          btmP,  nearP, farP;
      };
    };

    Plane& operator[](int rhs) { return planes[rhs]; }
    Plane const& operator[](int rhs) const { return planes[rhs]; }
  };

}

#include <pch.h>
#include "Ellipsoid.h"
#include <BoundingVolumes/BVHelpers.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#define PLAN_B

namespace BV
{

  Ellipsoid::Ellipsoid(glm::vec3 const& _center, glm::vec3 const _halfAxes, float _d) : Collider(),
    rotationMtx{}, center{ _center }, halfAxes{ _halfAxes }, d{ _d }
  {
    SetModel(Graphics::MeshType::SPHERE);
  }

  void Ellipsoid::Update(Transform const& transform)
  {
    glm::vec3 const combinedScale{ transform.GetBaseScale() * transform.scale };
    glm::vec3 const worldPos{ combinedScale * (rotationMtx * center) + transform.pos };
    glm::vec3 const worldHalfAxes{ combinedScale * halfAxes };

    // scale
    transformMtx = {
      worldHalfAxes.x, 0.f, 0.f, 0.f,
      0.f, worldHalfAxes.y, 0.f, 0.f,
      0.f, 0.f, worldHalfAxes.z, 0.f,
      0.f, 0.f, 0.f, 1.f
    };

    // rot
    transformMtx = glm::mat4(rotationMtx) * transformMtx;

    // transform
    transformMtx[3] = glm::vec4(worldPos, 1.f);
  }

  void Ellipsoid::ComputeBV(Graphics::VertexAttribs const& vtxAttribs)
  {
    glm::mat3 eigenVectors;
    glm::mat3 covarMtx{ BVHelpers::CovarianceMtx(vtxAttribs.points) };
    BVHelpers::Jacobi(covarMtx, eigenVectors);

    glm::vec3 minExtents(FLT_MAX);
    glm::vec3 maxExtents(-FLT_MAX);

    for (const auto& point : vtxAttribs.points) {
      for (int i = 0; i < 3; ++i) {
        float projection = glm::dot(point, eigenVectors[i]);
        if (projection < minExtents[i]) minExtents[i] = projection;
        if (projection > maxExtents[i]) maxExtents[i] = projection;
      }
    }

    // construct the initial ellipsoid
    halfAxes = (maxExtents - minExtents) * 0.5f;
    center = (minExtents + maxExtents) * 0.5f;
    rotationMtx = std::move(eigenVectors);

    static auto Power2 = [](float val) noexcept { return val * val; };

    // go through all points, and if any are outside the BV, grow the ellipsoid to include it
    // do this until all points are contained
    for (glm::vec3 const& pt : vtxAttribs.points)
    {
      auto const newPt{ glm::transpose(rotationMtx) * (pt - center) };
      glm::vec3 inverseHalfAxes{ 1.f / halfAxes };
      // if point already contained, continue

      while (Power2(newPt.x * inverseHalfAxes.x) + Power2(newPt.y * inverseHalfAxes.y) + Power2(newPt.z * inverseHalfAxes.z) > 1.f)
      {
        halfAxes *= 1.001f;
        //d *= 1.01f;
        inverseHalfAxes = 1.f / halfAxes;
      }
    }
  }

};

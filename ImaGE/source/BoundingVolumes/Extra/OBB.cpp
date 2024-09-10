#include <pch.h>
#include "OBB.h"
#include <BoundingVolumes/BVHelpers.h>

namespace BV
{

  OBB::OBB(glm::vec3 const& _center, glm::vec3 const& _halfExtents) : Collider(),
    rotationMtx{}, center{ _center }, halfExtents{ _halfExtents }
  {
    SetModel(Graphics::MeshType::CUBE);
  }

  void OBB::Update(Transform const& transform)
  {
    glm::vec3 const combinedScale{ transform.GetBaseScale() * transform.scale };
    glm::vec3 const worldPos{ combinedScale * (rotationMtx * center) + transform.pos };
    glm::vec3 const worldHalfExtents{ combinedScale * halfExtents };

    // scale
    transformMtx = {
      worldHalfExtents.x, 0.f, 0.f, 0.f,
      0.f, worldHalfExtents.y, 0.f, 0.f,
      0.f, 0.f, worldHalfExtents.z, 0.f,
      0.f, 0.f, 0.f, 1.f
    };

    transformMtx = glm::mat4(rotationMtx) * transformMtx;

    // transform
    transformMtx[3] = glm::vec4(worldPos, 1.f);
  }

  void OBB::ComputeBV(Graphics::VertexAttribs const& vtxAttribs)
  {
    glm::mat3 eigenVectors;
    glm::mat3 covarMtx{ BVHelpers::CovarianceMtx(vtxAttribs.points) };
    BVHelpers::Jacobi(covarMtx, eigenVectors);

    glm::vec3 minExtents, maxExtents;
    for (int i = 0; i < 3; ++i) {
      glm::vec3 const& axis = eigenVectors[i];

      int imin, imax;
      BVHelpers::ExtremePointsAlongDirection(axis, vtxAttribs.points, &imin, &imax);
      minExtents[i] = glm::dot(vtxAttribs.points[imin], axis);
      maxExtents[i] = glm::dot(vtxAttribs.points[imax], axis);
    }

    // Compute OBB center and half extents
    center = (minExtents + maxExtents) * 0.5f;
    halfExtents = (maxExtents - minExtents) * 0.5f;
    rotationMtx = std::move(eigenVectors);
  }

} // namespace BV

#pragma once
#include <BoundingVolumes/Collider.h>

namespace BV
{

  struct Ellipsoid : Collider
  {
    glm::mat3 rotationMtx;
    glm::vec3 center, halfAxes;
    float d;

    Ellipsoid(glm::vec3 const& _center, glm::vec3 const _halfAxes, float _d = 1.f);

    void Update(Transform const& transform) override;
    void ComputeBV(Graphics::VertexAttribs const& vtxAttribs) override;
    void Reset() override {}
  };

} // namespace BV

#pragma once
#include <BoundingVolumes/Collider.h>

namespace BV
{

  struct OBB : Collider
  {
    glm::mat3 rotationMtx;
    glm::vec3 center, halfExtents;

    OBB(glm::vec3 const& _center, glm::vec3 const& _halfExtents);

    void Update(Transform const& transform) override;
    void ComputeBV(Graphics::VertexAttribs const& vtxAttribs) override;
    void Reset() override {}
  };

} // namespace BV

#pragma once
#include <BoundingVolumes/Collider.h>

namespace BV
{

  struct AABB : Collider
  {
    AABB();
    AABB(glm::vec3 const& _center, glm::vec3 const& _halfExtents);

    // returns the bottom-left of the AABB
    inline glm::vec3 GetMin() const noexcept { return position - halfExtents; }
    inline glm::vec3 GetWorldMin() const noexcept { return worldPos - worldHalfExtents; }
    // returns the top-right of the AABB
    inline glm::vec3 GetMax() const noexcept { return position + halfExtents; }
    inline glm::vec3 GetWorldMax() const noexcept { return worldPos + worldHalfExtents; }

    float GetWorldVolume() const noexcept;
    void UpdateTransformWithWorld();

    void Update(Transform const& transform) override;
    void ComputeBV(Graphics::VertexAttribs const& vtxAttribs) override;
    void Reset() override {}

    glm::vec3 position;
    glm::vec3 halfExtents;
    glm::vec3 worldPos, worldHalfExtents;
  };

} // namespace BV
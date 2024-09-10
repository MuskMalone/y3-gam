#include <pch.h>
#include "AABB.h"

namespace BV
{
  AABB::AABB() : Collider() {
    SetModel(Graphics::MeshType::CUBE);
  }

  AABB::AABB(glm::vec3 const& _center, glm::vec3 const& _halfExtents) : Collider(), position{ _center }, halfExtents{ _halfExtents },
    worldPos{}, worldHalfExtents{}
  {
    SetModel(Graphics::MeshType::CUBE);
  }

  void AABB::Update(Transform const& transform)
  {
    glm::vec3 const combinedScale{ transform.GetBaseScale() * transform.scale };

    // obj.transform * this->transform
    worldPos = combinedScale * position + transform.pos;
    worldHalfExtents = combinedScale * halfExtents;

    transformMtx = glm::mat4(
      worldHalfExtents.x, 0.f, 0.f, 0.f,
      0.f, worldHalfExtents.y, 0.f, 0.f,
      0.f, 0.f, worldHalfExtents.z, 0.f,
      worldPos.x, worldPos.y, worldPos.z, 1.f
    );
  }

  float AABB::GetWorldVolume() const noexcept
  {
    glm::vec3 const extents{ worldHalfExtents * 2.f };
    return extents.x * extents.y * extents.z;
  }

  void AABB::UpdateTransformWithWorld()
  {
    transformMtx = glm::mat4(
      worldHalfExtents.x, 0.f, 0.f, 0.f,
      0.f, worldHalfExtents.y, 0.f, 0.f,
      0.f, 0.f, worldHalfExtents.z, 0.f,
      worldPos.x, worldPos.y, worldPos.z, 1.f
    );
  }

  void AABB::ComputeBV(Graphics::VertexAttribs const& vtxAttribs)
  {
    static const glm::vec3 axes[]{ {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f} };
    auto const& vertices{ vtxAttribs.points };

    float minMax[6];
    for (int i{}; i < 3; ++i)
    {
      glm::vec3 const& axis{ axes[i] };
      float min{ FLT_MAX }, max{ -FLT_MAX };
      for (size_t j{}; j < vertices.size(); j += 3)
      {
        glm::vec3 const& currVtx{ *reinterpret_cast<glm::vec3 const*>(vertices.data() + j) };

        float const dist{ glm::dot(currVtx, axis) };
        if (dist < min) {
          min = dist;
        }
        else if (dist > max) {
          max = dist;
        }
      }
      minMax[i * 2] = min;
      minMax[i * 2 + 1] = max;
    }

    position = glm::vec3(minMax[0] + minMax[1], minMax[2] + minMax[3], minMax[4] + minMax[5]) * 0.5f;
    halfExtents = glm::vec3(minMax[1] - minMax[0], minMax[3] - minMax[2], minMax[5] - minMax[4]) * 0.5f;
  }

} // namespace BV
#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Component
{

  struct Transform
  {
    Transform() :
      worldMtx{}, parentWorldMtx{ glm::identity<glm::mat4>() }, rotation{ 1.f, 0.f, 0.f, 0.f }, worldRot{ 1.f, 0.f, 0.f, 0.f },
      position{}, scale{}, eulerAngles{}, worldPos{}, worldScale{ 1.f },
      modified{ true } {}
    Transform(glm::vec3 const& _pos, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {}) :
      worldMtx{}, parentWorldMtx{ glm::identity<glm::mat4>() }, rotation{ 1.f, 0.f, 0.f, 0.f }, worldRot{ 1.f, 0.f, 0.f, 0.f },
      position{}, scale{}, eulerAngles{}, worldPos{ _pos }, worldScale{ _scale },
      modified{ true } {}

    void SetLocalRotWithEuler(glm::vec3 const& degrees);
    glm::vec3 GetWorldEulerAngles() const;
    void SetLocalToWorld() noexcept;
    void ComputeWorldMtx();
    void Clear() noexcept;

    glm::mat4 worldMtx, parentWorldMtx;
    glm::quat rotation, worldRot;

    glm::vec3 position, scale, eulerAngles;  // local
    glm::vec3 worldPos, worldScale;
    bool modified;
  };

}

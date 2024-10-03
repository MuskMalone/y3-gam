#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Component
{

  struct Transform
  {
    Transform() :
      worldMtx{}, parentWorldMtx{ glm::identity<glm::mat4>() }, rotation{}, worldRot{},
      position{}, scale{}, worldPos{}, worldScale{ 1.f },
      modified{ true } {}
    Transform(glm::vec3 const& _pos, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {}) :
      worldMtx{}, parentWorldMtx{ glm::identity<glm::mat4>() }, rotation{}, worldRot{},
      position{}, scale{}, worldPos{ _pos }, worldScale{ _scale }, 
      modified{ true } {}

    inline glm::vec3 GetLocalEulerAngles() const { return glm::degrees(glm::eulerAngles(rotation)); }
    inline glm::vec3 GetWorldEulerAngles() const { return glm::degrees(glm::eulerAngles(worldRot)); }
    inline void SetLocalRotWithEuler(glm::vec3 const& degrees) { rotation = glm::radians(degrees); }
    inline void SetWorldRotWithEuler(glm::vec3 const& degrees) { worldRot = degrees; }

    void ComputeWorldMtx() {
      glm::mat4 trans{ glm::translate(glm::mat4{ 1.f }, worldPos) };
      glm::mat4 rot{ glm::toMat4(worldRot) };
      glm::mat4 scale{ glm::scale(glm::mat4{ 1.f }, worldScale) };

      worldMtx = trans * rot * scale;
    }

    inline void Clear() noexcept {
      position = scale = {};
      worldPos = worldScale = {};
      rotation = worldRot = {};
      worldMtx = parentWorldMtx = glm::identity<glm::mat4>();
      modified = true;
    }

    glm::mat4 worldMtx, parentWorldMtx;
    glm::quat rotation, worldRot;

    glm::vec3 position, scale;  // local
    glm::vec3 worldPos, worldScale;
    bool modified;
  };

}

#pragma once
#include <glm/glm.hpp>

namespace Component
{

  struct Transform
  {
    Transform() :
      worldMtx{}, parentWorldMtx{ glm::identity<glm::mat4>() },
      position{}, scale{}, rotation{}, worldPos{}, worldScale{ 1.f }, worldRot{},
      modified{ true } {}
    Transform(glm::vec3 const& _pos, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {}) :
      worldMtx{}, parentWorldMtx{ glm::identity<glm::mat4>() },
      position{}, scale{}, rotation{}, worldPos{ _pos }, worldScale{ _scale }, worldRot{ _rot },
      modified{ true } {}

    inline void Clear() noexcept {
      position = scale = rotation = glm::vec3();
      worldPos = worldScale = worldRot = glm::vec3();
      modified = true;
    }

    glm::mat4 worldMtx, parentWorldMtx;
    glm::vec3 position, scale, rotation;  // local values
    glm::vec3 worldPos, worldScale, worldRot;
    bool modified;
    //glm::quat rot; //change rot to quat
  };

}

#include <pch.h>
#include "CameraSpec.h"
#include <Graphics/Utils.h>

namespace Graphics {
  CameraSpec::CameraSpec(glm::vec3 const& pos, float yaw, float pitch,
    float _fov, float ar, float _nearClip, float _farClip, bool editorMode) :
    viewMatrix{ Utils::Camera::GetViewMatrix(position, yaw, pitch) },
    viewProjMatrix{ Utils::Camera::GetProjMatrix(_fov, aspectRatio, _nearClip, _farClip) * viewMatrix },
    position{ pos }, rotation{}, nearClip{ _nearClip }, farClip{ _farClip }, fov{ _fov }, aspectRatio{ ar }, isEditor{ editorMode }
  {
    ComputeFrustum();
  }

  void CameraSpec::ComputeFrustum() {
    frustum = Utils::Culling::ComputeFrustum(viewProjMatrix);
  }
}

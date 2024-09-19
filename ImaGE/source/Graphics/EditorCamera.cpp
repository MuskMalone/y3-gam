#include <pch.h>
#include "EditorCamera.h"

namespace Graphics {
    glm::mat4 EditorCamera::GetViewMatrix() const {
        return Utils::Camera::GetViewMatrix(mPosition, mYaw, mPitch);
    }

    glm::mat4 EditorCamera::GetProjMatrix() const {
        return Utils::Camera::GetProjMatrix(mFov, mAspectRatio, mNearClip, mFarClip);
    }

    glm::mat4 EditorCamera::GetViewProjMatrix() const {
        return GetProjMatrix() * GetViewMatrix();
    }
}
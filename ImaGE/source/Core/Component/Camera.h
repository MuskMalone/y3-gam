#pragma once
#include <pch.h>
#include "Graphics/Utils.h"
namespace Component {
    struct Camera {
        glm::vec3 position{ glm::vec3{ 0.0f, 0.0f, 3.0f } };
        float yaw{ -90.0f };
        float pitch{ 0.0f };
        float fov{ 45.0f };
        float aspectRatio{ 16.0f / 9.0f };
        float nearClip{ 0.1f };
        float farClip{ 100.0f };

        glm::mat4 GetViewMatrix() const {
            return Graphics::Utils::Camera::GetViewMatrix(position, yaw, pitch);
        }

        glm::mat4 GetProjMatrix() const {
            return Graphics::Utils::Camera::GetProjMatrix(fov, aspectRatio, nearClip, farClip);
        }

        glm::mat4 GetViewProjMatrix() const {
            return GetProjMatrix() * GetViewMatrix();
        }
    };
}

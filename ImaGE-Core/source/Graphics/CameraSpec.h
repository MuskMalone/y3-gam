// CameraSpec.h
#pragma once
#include <glm/glm.hpp>        // Include the EditorCamera class
#include "Core/Components/Camera.h"  // Include the Camera component struct

namespace Graphics {
 
    struct CameraSpec {
  // Forward declaration of EditorCamera
        glm::mat4 viewProjMatrix{};
        glm::mat4 viewMatrix{};
        glm::vec3 position{};
        glm::quat rotation{};
        float nearClip{};
        float farClip{};
        float fov{};
        float aspectRatio{};
        bool isEditor{};

        // Main constructor to initialize all fields directly
        CameraSpec(const glm::mat4& vpMatrix, const glm::mat4& vMatrix, const glm::vec3& pos, float nearC, float farC, float fieldOfView, float ar, bool editorMode = false)
            : viewProjMatrix{ vpMatrix }, viewMatrix{ vMatrix }, position{ pos }, rotation{}, nearClip {
            nearC
        }, farClip{ farC }, fov{ fieldOfView }, aspectRatio{ ar }, isEditor{ editorMode } {}

        // Constructor overload for Camera component
        CameraSpec(const Component::Camera& cameraComp)
            : viewProjMatrix{ cameraComp.GetViewProjMatrix() }, viewMatrix{cameraComp.GetViewMatrix()},
            position{ cameraComp.position },
            rotation{ cameraComp.rotation },
            nearClip{ cameraComp.nearClip },
            farClip{ cameraComp.farClip },
            fov{ cameraComp.fov },
            aspectRatio{ cameraComp.aspectRatio },
            isEditor{ false } {}
    };
}

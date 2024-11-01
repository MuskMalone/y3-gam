// CameraSpec.h
#pragma once
#include <glm/glm.hpp>        // Include the EditorCamera class
#include "Core/Components/Components.h"  // Include the Camera component struct

namespace Graphics {
 
    struct CameraSpec {
  // Forward declaration of EditorCamera
        glm::mat4 viewProjMatrix{};
        glm::vec3 position{};
        float nearClip{};
        float farClip{};
        float fov{};

        // Main constructor to initialize all fields directly
        CameraSpec(const glm::mat4& vpMatrix, const glm::vec3& pos, float nearC, float farC, float fieldOfView)
            : viewProjMatrix{ vpMatrix }, position{ pos }, nearClip{ nearC }, farClip{ farC }, fov{ fieldOfView } {}

        // Constructor overload for Camera component
        CameraSpec(const Component::Camera& cameraComp)
            : viewProjMatrix{ cameraComp.GetViewProjMatrix() },
            position{ cameraComp.position },
            nearClip{ cameraComp.nearClip },
            farClip{ cameraComp.farClip },
            fov{ cameraComp.fov } {}
    };
}

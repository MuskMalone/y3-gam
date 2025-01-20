// CameraSpec.h
#pragma once
#include <glm/glm.hpp>        // Include the EditorCamera class
#include "Core/Components/Camera.h"  // Include the Camera component struct
#include <BoundingVolumes/Frustum.h>
#include <rttr/rttr_enable.h>

namespace Graphics {
 
    struct CameraSpec {
    protected:
        BV::Frustum frustum;

        void ComputeFrustum();
    public:
        // Forward declaration of EditorCamera
        glm::mat4 viewMatrix;
        glm::mat4 viewProjMatrix;
        glm::vec3 position;
        glm::quat rotation;
        float nearClip, farClip;
        float fov;
        float aspectRatio;
        bool isEditor;

        CameraSpec() = default;
        // Main constructor to initialize all fields directly
        CameraSpec(glm::vec3 const& position, float yaw, float pitch,
          float fov, float aspectRatio, float nearClip, float farClip, bool editorMode = false);

        // Constructor overload for Camera component
        CameraSpec(const Component::Camera& cameraComp)
            : viewMatrix{ cameraComp.GetViewMatrix() }, viewProjMatrix{ cameraComp.GetViewProjMatrix() },
            position{ cameraComp.position }, rotation{ cameraComp.rotation },
            nearClip{ cameraComp.nearClip }, farClip{ cameraComp.farClip },
            fov{ cameraComp.fov }, aspectRatio{ cameraComp.aspectRatio }, isEditor{ false } {
          ComputeFrustum();
        }

        inline glm::vec3 GetUpVector() const { return glm::vec3(0.0f, 1.0f, 0.0f); }
        inline glm::vec3 GetForwardVector() const { return rotation * glm::vec3(0.f, 0.f, -1.f); }
        inline glm::vec3 GetRightVector() const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f); }

        BV::Frustum const& GetFrustum() const { return frustum; }

        RTTR_ENABLE()
    };
}

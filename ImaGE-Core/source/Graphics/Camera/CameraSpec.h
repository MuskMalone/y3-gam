// CameraSpec.h
#pragma once
#include <glm/glm.hpp>        // Include the EditorCamera class
#include "Core/Components/Camera.h"  // Include the Camera component struct
#include <BoundingVolumes/Frustum.h>

namespace Graphics {
 
    struct CameraSpec {
    private:
        BV::Frustum frustum;

        void ComputeFrustum();
    public:
        // Forward declaration of EditorCamera
        glm::mat4 viewProjMatrix;
        glm::mat4 viewMatrix;
        glm::vec3 position;
        glm::quat rotation;
        float nearClip, farClip;
        float fov;
        float aspectRatio;
        bool isEditor;

        CameraSpec() = default;
        // Main constructor to initialize all fields directly
        CameraSpec(const glm::mat4& vpMatrix, const glm::mat4& vMatrix, const glm::vec3& pos, float nearC, float farC, float fieldOfView, float ar, bool editorMode = false)
            : viewProjMatrix{ vpMatrix }, viewMatrix{ vMatrix }, position{ pos }, rotation{},
          nearClip { nearC }, farClip{ farC }, fov{ fieldOfView }, aspectRatio{ ar }, isEditor{ editorMode } {
          ComputeFrustum();
        }

        // Constructor overload for Camera component
        CameraSpec(const Component::Camera& cameraComp)
            : viewProjMatrix{ cameraComp.GetViewProjMatrix() }, viewMatrix{cameraComp.GetViewMatrix()},
            position{ cameraComp.position }, rotation{ cameraComp.rotation },
            nearClip{ cameraComp.nearClip }, farClip{ cameraComp.farClip },
            fov{ cameraComp.fov }, aspectRatio{ cameraComp.aspectRatio }, isEditor{ false } {
          ComputeFrustum();
        }

        inline glm::vec3 GetUpVector() const { return glm::vec3(0.0f, 1.0f, 0.0f); }
        inline glm::vec3 GetForwardVector() const { return rotation * glm::vec3(0.f, 0.f, -1.f); }
        inline glm::vec3 GetRightVector() const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f); }

        BV::Frustum const& GetFrustum() const { return frustum; }
    };
}

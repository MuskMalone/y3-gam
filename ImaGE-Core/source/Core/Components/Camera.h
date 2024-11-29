#pragma once
#include "Graphics/Utils.h"
//#include <BoundingVolumes/Frustum.h>

namespace Component {

    struct Camera {
        enum class Type {
            PERSP,
            ORTHO
        };
        glm::vec3 position{ glm::vec3{ 0.0f, 0.0f, -10.0f } };
        glm::quat rotation{ glm::quat{glm::vec3{0.f}} };
        Type projType{ Type::PERSP };
        float yaw{ 0.0f };
        float pitch{ 0.0f };
        float fov{ 45.0f };
        float aspectRatio{ 16.0f / 9.0f }; //@TODO CHANGE
        float nearClip{ 0.1f };
        float farClip{ 100.0f };

        // Orthographic bounds
        float left{ -aspectRatio * UI_SCALING_FACTOR<float> };
        float right{ aspectRatio * UI_SCALING_FACTOR<float> };
        float bottom{ -10.0f };
        float top{ 10.0f };

        inline void Clear() noexcept {
            projType = Type::PERSP;
            position = { 0.f ,0.f , -10.f };
            yaw = 0.f;
            pitch = 0.f;
            fov = 45.f;
            
            aspectRatio = 16.f/9.f;
            nearClip = .1f;
            farClip = 100.f;

            // Reset orthographic bounds
            left = -aspectRatio * UI_SCALING_FACTOR<float>;
            right = aspectRatio * UI_SCALING_FACTOR<float>;
            bottom = -10.0f;
            top = 10.0f;
        }

        inline glm::mat4 GetViewMatrix() const {
            //glm::vec3 front{
            //    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            //    sin(glm::radians(pitch)),
            //    sin(glm::radians(yaw)) * cos(glm::radians(pitch))
            //};
            //glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
            //glm::vec3 up = glm::normalize(glm::cross(right, front));
            //return glm::lookAt(position, position + front, up);

            return glm::lookAt(position, position + GetForwardVector(), GetUpVector());
        }

        inline glm::mat4 GetProjMatrix() const {
            if (projType == Type::PERSP) {
                return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
            }
            else {
                // Example values for orthographic projection
                float left = -aspectRatio * UI_SCALING_FACTOR<float>;
                float right = aspectRatio * UI_SCALING_FACTOR<float>;
                float bottom = -10.0f;
                float top = 10.0f;
                return glm::ortho(left, right, bottom, top, nearClip, farClip);
            }
        }

        inline glm::mat4 GetViewProjMatrix() const {
            return GetProjMatrix() * GetViewMatrix();
        }

        // Provide a utility function to get orthographic bounds
        inline glm::vec4 GetOrthographicBounds() const {
            return glm::vec4(left, right, bottom, top);
        }

        inline glm::vec3 GetUpVector() const { return glm::vec3(0.0f, 1.0f, 0.0f); }
        inline glm::vec3 GetForwardVector() const { return rotation * glm::vec3(0.f, 0.f, -1.f); }
        inline glm::vec3 GetRightVector() const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f); }

        //void ComputeFrustum() {
        //  float const halfV{ farClip * tanf(fov * 0.5f) }, halfH{ halfV * aspectRatio };
        //  glm::vec3 const fwdVec{ GetForwardVector() }, rightVec{ GetRightVector() },
        //    upVec{ GetUpVector() };
        //  glm::vec3 const fwdFarProd{ farClip * fwdVec };

        //  frustum.topP = { fwdVec * nearClip + position, fwdVec };
        //  frustum.btmP = { position + fwdFarProd, -fwdVec };

        //  glm::vec3 vecFwdFarProd{ rightVec * halfH };
        //  frustum.leftP = { position, glm::cross(fwdFarProd - vecFwdFarProd, GetUpVector()) };
        //  frustum.rightP = { position, glm::cross(upVec, fwdFarProd + vecFwdFarProd) };

        //  vecFwdFarProd = upVec * halfV;
        //  frustum.nearP = { position, glm::cross(fwdFarProd + vecFwdFarProd, rightVec) };
        //  frustum.farP = { position, glm::cross(rightVec, fwdFarProd - vecFwdFarProd) };
        //}
    };
}

#pragma once
#include "Graphics/Utils.h"

namespace Component {

    struct Camera {
        enum class Type {
            PERSP,
            ORTHO
        };
        Type projType{ Type::PERSP };
        glm::vec3 position{ glm::vec3{ 0.0f, 0.0f, -10.0f } };
        glm::quat rotation{ glm::quat{glm::vec3{0.f}} };
        float yaw{ 0.0f };
        float pitch{ 0.0f };
        float fov{ 45.0f };
        float aspectRatio{ 16.0f / 9.0f }; //@TODO CHANGE
        float nearClip{ 0.1f };
        float farClip{ 100.0f };

        inline void Clear() noexcept {
            projType = Type::PERSP;
            position = { 0.f ,0.f , -10.f };
            yaw = 0.f;
            pitch = 0.f;
            fov = 45.f;
            
            aspectRatio = 16.f/9.f;
            nearClip = .1f;
            farClip = 100.f;
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
            glm::vec3 front = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);

            return glm::lookAt(position, position + front, up);
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
    };
}

#pragma once
#include "Renderer.h"

namespace Graphics {
	namespace Utils{

        class RenderContext {

        public:
            RenderContext(glm::mat4 const& viewProjMatrix) {
                Renderer::RenderSceneBegin(viewProjMatrix);
            }

            ~RenderContext() {
                Renderer::RenderSceneEnd();
            }

            RenderContext() = delete;
            RenderContext(const RenderContext&) = delete;
            RenderContext& operator=(const RenderContext&) = delete;
        };

		namespace Camera {
            inline glm::mat4 GetViewMatrix(glm::vec3 const& position, float yaw, float pitch) {
                // Calculate the view matrix using position, yaw, pitch
                glm::vec3 front{
                    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                    sin(glm::radians(pitch)),
                    sin(glm::radians(yaw)) * cos(glm::radians(pitch))
                };
                glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
                glm::vec3 up = glm::normalize(glm::cross(right, front));
                return glm::lookAt(position, position + front, up);
            }

            inline glm::mat4 GetProjMatrix(float fov, float aspectRatio, float nearClip, float farClip){
                return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
            }
		}


	};
}

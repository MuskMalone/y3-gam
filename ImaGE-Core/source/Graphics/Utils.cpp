#include <pch.h>
#include "Utils.h"
#include "Renderer.h"
#include <Core/Components/Transform.h>
#include <BoundingVolumes/IntersectionTests.h>
#include <Graphics/Mesh/MeshSource.h>

namespace Graphics {
    namespace Utils {

        RenderContext::RenderContext(glm::mat4 const& viewProjMatrix) {
            Renderer::RenderSceneBegin(viewProjMatrix);
        }

        RenderContext::~RenderContext() {
            Renderer::RenderSceneEnd();
        }

        namespace Camera {
            glm::mat4 GetViewMatrix(glm::vec3 const& position, float yaw, float pitch) {
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

            glm::mat4 GetProjMatrix(float fov, float aspectRatio, float nearClip, float farClip) {
                return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
            }
        }

        namespace Framebuffer {
            bool IsDepthFormat(FramebufferTextureFormat fmt) {
                switch (fmt) {
                case FramebufferTextureFormat::DEPTH24STENCIL8:
                case FramebufferTextureFormat::SHADOW_MAP:
                  return true;
                default:
                    return false;
                }
            }

            void CreateTextures(uint32_t* id, uint32_t count) {
                glCreateTextures(GL_TEXTURE_2D, count, id);
            }

            void BindTexture(uint32_t id) {
                glBindTexture(GL_TEXTURE_2D, id);
            }

            void AttachColorTexture(uint32_t id, GLenum intFmt, GLenum fmt, uint32_t width, uint32_t height, int index) {
                glTexImage2D(GL_TEXTURE_2D, 0, intFmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
            }

            void AttachDepthTexture(uint32_t id, GLenum intFmt, GLenum attachType, uint32_t width, uint32_t height) {
                glTexStorage2D(GL_TEXTURE_2D, 1, intFmt, width, height);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, id, 0);
            }

            void AttachShadowMapTexture(uint32_t& id, uint32_t width, uint32_t height) {
              glGenTextures(1, &id);
              glBindTexture(GL_TEXTURE_2D, id);
              glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
              //glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT, width, height);
              
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

              // prevent darkness outside the frustrum
              float clampColor[] = { 1.f, 1.f, 1.f, 1.f };
              glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

              glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
              // since we don't touch the color buffer
          /*    glDrawBuffer(GL_NONE);
              glReadBuffer(GL_NONE);*/

              glBindTexture(GL_TEXTURE_2D, 0);
            }

        }//namespace Framebuffer

        namespace GL {

            std::set<GLenum> ExistingErrors;

            bool GlLogCall(const char* function, const char* file, int line)
            {
              
                bool success = true;
                while (GLenum error = glGetError())
                {
                    // If the error has already been logged, skip it
                    if (ExistingErrors.find(error) != ExistingErrors.end())
                        continue;

                    // Mark the error as logged
                    ExistingErrors.insert(error);

                    // Log the error
                    success = false;
#ifdef _DEBUG
                    std::stringstream ss;
                    ss << "[OpenGL Error] (" << error << "): " << function
                        << " in file " << file << " at line " << line << std::endl;

                    std::cout << ss.str() << std::endl;
#endif

                    //Debug::DebugLogger::GetInstance().LogError(ss.str());
                }
                return success;
                
            }

            void GlClearErrors() {
                while (glGetError() != GL_NO_ERROR);
            }

        }//namespace GL

        namespace Culling {
          // Gribb/Hartmann method
          // source: https://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
          BV::Frustum ComputeFrustum(glm::mat4 const& viewProjMatrix) {
            BV::Frustum frustum{};
            frustum.leftP = { viewProjMatrix[0][3] + viewProjMatrix[0][0],
                      viewProjMatrix[1][3] + viewProjMatrix[1][0],
                      viewProjMatrix[2][3] + viewProjMatrix[2][0],
                      viewProjMatrix[3][3] + viewProjMatrix[3][0]
            };
            frustum.rightP = { viewProjMatrix[0][3] - viewProjMatrix[0][0],
                               viewProjMatrix[1][3] - viewProjMatrix[1][0],
                               viewProjMatrix[2][3] - viewProjMatrix[2][0],
                               viewProjMatrix[3][3] - viewProjMatrix[3][0]
            };

            frustum.topP = { viewProjMatrix[0][3] - viewProjMatrix[0][1],
                             viewProjMatrix[1][3] - viewProjMatrix[1][1],
                             viewProjMatrix[2][3] - viewProjMatrix[2][1],
                             viewProjMatrix[3][3] - viewProjMatrix[3][1]
            };
            frustum.btmP = { viewProjMatrix[0][3] + viewProjMatrix[0][1],
                             viewProjMatrix[1][3] + viewProjMatrix[1][1],
                             viewProjMatrix[2][3] + viewProjMatrix[2][1],
                             viewProjMatrix[3][3] + viewProjMatrix[3][1]
            };

            frustum.nearP = { viewProjMatrix[0][3] + viewProjMatrix[0][2],
                              viewProjMatrix[1][3] + viewProjMatrix[1][2],
                              viewProjMatrix[2][3] + viewProjMatrix[2][2],
                              viewProjMatrix[3][3] + viewProjMatrix[3][2]
            };
            frustum.farP = { viewProjMatrix[0][3] - viewProjMatrix[0][2],
                             viewProjMatrix[1][3] - viewProjMatrix[1][2],
                             viewProjMatrix[2][3] - viewProjMatrix[2][2],
                             viewProjMatrix[3][3] - viewProjMatrix[3][2]
            };

            /*for (int i{}; i < 6; ++i) {
              frustum[i].Normalize();
            }*/

            return frustum;
          }

          // from learnopengl
          bool EntityInViewFrustum(BV::Frustum const& frustum, Component::Transform const& transform, Graphics::MeshSource const& meshSource) {
            BV::AABB aabb{ transform.worldPos, glm::vec3(1.f) };
            // apply scale to halfExtents of mesh
            glm::vec3 const& halfExt{ meshSource.GetBoundingBox().halfExtents };

            // if rotation is identity (unmodified), simply multiply half extents to scale
            glm::vec3 const right = transform.worldMtx[0] * halfExt.x;
            glm::vec3 const up = transform.worldMtx[1] * halfExt.y;
            glm::vec3 const forward = -transform.worldMtx[2] * halfExt.z;

            aabb.halfExtents.x = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
              std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
              std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

            aabb.halfExtents.y = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
              std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
              std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

            aabb.halfExtents.z = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
              std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
              std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

            return BV::FrustumAABBIntersection(frustum, aabb);
          }
        }
    };
}

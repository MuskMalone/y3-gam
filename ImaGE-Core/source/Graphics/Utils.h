#pragma once
#include "Renderer.h"
#include "Framebuffer.h"

#define GLCALL(x) Graphics::Utils::GL::GlClearErrors(); x; Graphics::Utils::GL::GlLogCall(#x, __FILE__, __LINE__)

//#define GLCALL(x) x;

namespace Graphics {
	namespace Utils{

        class RenderContext {

        public:
            RenderContext(glm::mat4 const& viewProjMatrix);

            ~RenderContext();

            RenderContext() = delete;
            RenderContext(const RenderContext&) = delete;
            RenderContext& operator=(const RenderContext&) = delete;
        };

		namespace Camera {
            glm::mat4 GetViewMatrix(glm::vec3 const& position, float yaw, float pitch);
            glm::mat4 GetProjMatrix(float fov, float aspectRatio, float nearClip, float farClip);
		}//namespace Camera

        namespace Framebuffer {
            bool IsDepthFormat(FramebufferTextureFormat fmt);
            void CreateTextures(uint32_t* id, uint32_t count);
            void BindTexture(uint32_t id);
            void AttachColorTexture(uint32_t id, GLenum intFmt, GLenum fmt, uint32_t width, uint32_t height, int index);
            void AttachDepthTexture(uint32_t id, GLenum intFmt, GLenum attachType, uint32_t width, uint32_t height);
        } //namespace Framebuffer

        namespace GL {
            bool GlLogCall(const char* function, const char* file, int line);
            void GlClearErrors();

        }//GL
	};

} //namespace Graphics

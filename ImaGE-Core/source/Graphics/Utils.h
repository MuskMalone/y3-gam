/*!*********************************************************************
\file   Utils.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Utils namespace contains several utility functions and helper classes used throughout
        the graphics engine. These utilities handle tasks such as setting up the view and projection
        matrices for cameras, managing framebuffer textures, and logging OpenGL errors.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
//#include "Renderer.h"
#include "Framebuffer.h"
#include <glm/glm.hpp>

#ifndef NDEBUG
// Debug or other specific mode: error checking and logging
#define GLCALL(x) Graphics::Utils::GL::GlClearErrors(); x; Graphics::Utils::GL::GlLogCall(#x, __FILE__, __LINE__)
#else
// Release or other non-debug mode: no error checking
#define GLCALL(x) x
#endif

// forward declarations
namespace BV { struct Frustum; }
namespace Graphics { class MeshSource; }
namespace Component { struct Transform; }

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
            void AttachColorTexture(uint32_t id, GLenum intFmt, GLenum fmt, uint32_t width, uint32_t height, int index, GLenum type = GL_UNSIGNED_BYTE);
            void AttachDepthTexture(uint32_t id, GLenum intFmt, GLenum attachType, uint32_t width, uint32_t height);
            void AttachShadowMapTexture(uint32_t& id, uint32_t width, uint32_t height);
        } //namespace Framebuffer

        namespace GL {
            bool GlLogCall(const char* function, const char* file, int line);
            void GlClearErrors();

        }//GL

        namespace Culling {
          BV::Frustum ComputeFrustum(glm::mat4 const& viewProjMtx);
          bool EntityInViewFrustum(BV::Frustum const& frustum, Component::Transform const& transform, Graphics::MeshSource const& meshSource);
        }
	};

} //namespace Graphics

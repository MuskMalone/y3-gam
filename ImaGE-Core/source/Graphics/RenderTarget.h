#pragma once
#include <Scenes/Scene.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/EditorCamera.h>

namespace Graphics {
  struct RenderTarget {
    RenderTarget(FramebufferSpec const& spec) :
        camera{}, framebuffer{ std::make_shared<Framebuffer>(spec) } {}

    EditorCamera camera;
    std::shared_ptr<Framebuffer> framebuffer;
  };
} // namespace Graphics

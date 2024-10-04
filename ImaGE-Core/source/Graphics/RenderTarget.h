#pragma once
#include <Scenes/Scene.h>
#include <Graphics/Framebuffer.h>

namespace Graphics {
  struct RenderTarget {
    RenderTarget(Graphics::FramebufferSpec const& spec) :
      framebuffer{ std::make_shared<Graphics::Framebuffer>(spec) }, scene{} {}

    std::shared_ptr<Graphics::Framebuffer> framebuffer;
    Scene scene;
  };
} // namespace Graphics

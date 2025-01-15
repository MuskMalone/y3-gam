#pragma once
#include <Scenes/Scene.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/Camera/EditorCamera.h>

namespace Graphics {
  struct RenderTarget {
    RenderTarget(FramebufferSpec const& spec) :
      camera{ std::make_shared<EditorCamera>() },
      framebuffer{std::make_shared<Framebuffer>(spec)} {}
    RenderTarget(FramebufferSpec const& spec, std::shared_ptr<EditorCamera>&& cam) :
      camera{ std::move(cam) },
      framebuffer{ std::make_shared<Framebuffer>(spec) } {}

    std::shared_ptr<EditorCamera> camera;
    std::shared_ptr<Framebuffer> framebuffer;
  };
} // namespace Graphics

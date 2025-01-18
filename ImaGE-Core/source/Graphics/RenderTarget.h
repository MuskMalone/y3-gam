#pragma once
#include <Scenes/Scene.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/Camera/EditorCamera.h>

namespace Graphics {
  // only holds a camera for now since fb is handled by render passes
  struct RenderTarget {
    RenderTarget() : camera{ std::make_shared<EditorCamera>() } {}
    RenderTarget(std::shared_ptr<EditorCamera>&& cam) : camera{ std::move(cam) } {}

    std::shared_ptr<EditorCamera> camera;
  };
} // namespace Graphics

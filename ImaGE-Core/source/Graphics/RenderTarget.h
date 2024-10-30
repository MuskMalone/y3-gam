#pragma once
#include <Scenes/Scene.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/EditorCamera.h>

namespace Graphics {
  struct RenderTarget {
    RenderTarget(FramebufferSpec const& spec, bool editorView = false) :
        camera{}, framebuffer{ std::make_shared<Framebuffer>(spec) }, isEditorView{editorView} {}

    EditorCamera camera;
    std::shared_ptr<Framebuffer> framebuffer;
    bool isEditorView;
  };
} // namespace Graphics

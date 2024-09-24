#pragma once
#ifndef IMGUI_DISABLE
#include <vector>
#include <memory>
#include <GUI/GUIWindow.h>
#include "Styles/Styler.h"
#include <Graphics/Framebuffer.h>
#include <Core/Entity.h>

namespace GUI {
  class Viewport;

  class GUIManager {
  public:

    GUIManager();

    // taking in framebuffer to pass into viewport class
    void Init();
    void UpdateGUI(std::shared_ptr<Temp::Graphics::Framebuffer> const& framebuffer);

    static inline Styler& GetStyler() noexcept { return mStyler; }
    static inline ECS::Entity const& GetSelectedEntity() noexcept { return sSelectedEntity; }
    static inline void SetSelectedEntity(ECS::Entity const& entity) noexcept { sSelectedEntity = entity; }
    
  private:
    std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    std::vector<std::shared_ptr<GUIWindow>> mWindows; // dockable/hideable windows
    std::shared_ptr<Viewport> mEditorViewport;  // ptr to the viewport in mWindows

    static Styler mStyler; // handles editor's styles
    static ECS::Entity sSelectedEntity; // currently selected entity
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

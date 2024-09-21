#pragma once
#ifndef IMGUI_DISABLE
#include <vector>
#include <memory>
#include <GUI/GUIWindow.h>
#include <Graphics/Framebuffer.h>
#include <Core/Entity.h>

namespace GUI
{
  class GUIManager
  {
  public:

    GUIManager();

    // taking in framebuffer to pass into viewport class
    void Init(Graphics::Temp::Framebuffer const& framebuffer);
    void UpdateGUI();
    void StyleGUI() const;

    static inline ECS::Entity const& GetSelectedEntity() noexcept { return sSelectedEntity; }
    static inline void SetSelectedEntity(ECS::Entity const& entity) noexcept { sSelectedEntity = entity; }

  private:
    std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    std::vector<std::unique_ptr<GUIWindow>> mWindows; // dockable/hideable windows

    static ECS::Entity sSelectedEntity; // currently selected entity
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

#pragma once
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
    // taking in framebuffer to pass into viewport class
    static void Init(Graphics::Framebuffer const& framebuffer);
    static void UpdateGUI();

    static inline ECS::Entity const& GetSelectedEntity() noexcept { return mSelectedEntity; }
    static inline void SetSelectedEntity(ECS::Entity const& entity) noexcept { mSelectedEntity = entity; }

  private:
    static std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    static std::vector<std::unique_ptr<GUIWindow>> mWindows; // dockable/hideable windows
    static ECS::Entity mSelectedEntity; // currently selected entity
  };

} // namespace GUI

#pragma once
#include <vector>
#include <memory>
#include <GUI/GUIWindow.h>
#include <Graphics/Framebuffer.h>

namespace GUI
{
  class GUIManager
  {
  public:
    // taking in framebuffer to pass into viewport class
    static void Init(Graphics::Framebuffer const& framebuffer);
    static void UpdateGUI();

  private:
    static std::vector<std::unique_ptr<GUIWindow>> m_persistentElements;  // contains controls outside of the dockspace
    static std::vector<std::unique_ptr<GUIWindow>> m_windows; // dockable/hideable windows
  };

} // namespace GUI

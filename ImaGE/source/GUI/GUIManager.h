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
    static void Init(Graphics::Framebuffer const& framebuffer);
    static void UpdateGUI();

  private:
    static std::vector<std::unique_ptr<GUIWindow>> m_windows;

    static void RunToolbar();
  };

} // namespace GUI

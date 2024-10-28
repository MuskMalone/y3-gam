#pragma once
#include <GUI/GUIWindow.h>

namespace GUI {
  class RenderPassViewer : public GUIWindow
  {
  public:
    RenderPassViewer(const char* windowName);
    void Run() override;

  private:

  };
} // namespace GUI

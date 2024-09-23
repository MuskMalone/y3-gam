#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>

namespace GUI {
  class Console : public GUIWindow {
  public:
    Console(std::string const& name);

    void Run() override;

  private:

  };

} // namespace GUI
#endif  // IMGUI_DISABLE

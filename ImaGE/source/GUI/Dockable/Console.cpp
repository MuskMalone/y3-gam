#include "pch.h"
#ifndef IMGUI_DISABLE
#include "Console.h"
#include <ImGui/imgui.h>

namespace GUI {

  Console::Console(std::string const& name) : GUIWindow(name) {}
  
  void Console::Run() {
    ImGui::Begin(mWindowName.c_str());



    ImGui::End();
  }


}

#endif  // IMGUI_DISABLE

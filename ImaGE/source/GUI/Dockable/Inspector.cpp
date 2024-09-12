#include <pch.h>
#include "Inspector.h"
#include <imgui/imgui.h>

namespace GUI
{

  Inspector::Inspector(std::string const& name) : GUIWindow(name) {}

  void Inspector::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    

    ImGui::End();
  }

} // namespace GUI

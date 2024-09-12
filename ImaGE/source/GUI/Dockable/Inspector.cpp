#include <pch.h>
#include "Inspector.h"
#include <imgui/imgui.h>

namespace GUI
{

  Inspector::Inspector(std::string const& name) : GUIWindow(name) {}

  void Inspector::Run()
  {
    ImGui::Begin(m_windowName.c_str());

    

    ImGui::End();
  }

} // namespace GUI

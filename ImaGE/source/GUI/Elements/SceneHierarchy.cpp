#include <pch.h>
#include "SceneHierarchy.h"
#include <imgui/imgui.h>

namespace GUI
{

  SceneHierarchy::SceneHierarchy(std::string const& name) : GUIWindow(name) {}

  void SceneHierarchy::Run()
  {
    ImGui::Begin(m_windowName.c_str());

    

    ImGui::End();
  }

} // namespace GUI

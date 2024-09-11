#include <pch.h>
#include "SceneControls.h"
#include <imgui/imgui.h>

namespace GUI
{

  SceneControls::SceneControls(std::string const& name) : GUIWindow(name) {}

  void SceneControls::Run()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::Button("Play"))
      {

      }

      if (ImGui::Button("Pause"))
      {

      }

      if (ImGui::Button("Step"))
      {

      }

      ImGui::EndMenuBar();
    }
  }

} // namespace GUI

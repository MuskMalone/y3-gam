#include <pch.h>
#include "SceneControls.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar

namespace GUI
{

  SceneControls::SceneControls(std::string const& name) : GUIWindow(name) {}

  void SceneControls::Run()
  {
    if (ImGui::BeginViewportSideBar("##SceneControls", (ImGuiViewportP*)(void*)ImGui::GetMainViewport(),
      ImGuiDir_Up, ImGui::GetFrameHeight(), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
    {
      if (ImGui::BeginMenuBar())
      {
        static float const xOffset{ (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("     PlayPauseStep").x) * 0.5f };
        ImGui::SetCursorPosX(xOffset);

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
    ImGui::End();
  }

} // namespace GUI

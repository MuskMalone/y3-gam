#include <pch.h>
#ifndef IMGUI_DISABLE
#include "SceneControls.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar
#include <Events/EventManager.h>

namespace GUI
{

  SceneControls::SceneControls(std::string const& name)
    : mSceneManager{ Scenes::SceneManager::GetInstance() }, GUIWindow(name) {}

  void SceneControls::Run()
  {
    bool const sceneStopped{ !mSceneManager.IsScenePlaying() };

    if (!sceneStopped) {
      ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.6f, 0.f, 1.f));
    }

    if (ImGui::BeginViewportSideBar("##SceneControls", (ImGuiViewportP*)(void*)ImGui::GetMainViewport(),
      ImGuiDir_Up, ImGui::GetFrameHeight(), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
    {
      if (ImGui::BeginMenuBar())
      {
        static float const xOffset{ (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("     PlayPauseStep").x) * 0.5f };
        bool const sceneNotPlaying{ mSceneManager.GetSceneState() != Scenes::SceneState::PLAYING };
        ImGui::SetCursorPosX(xOffset);

        // stop button
        ImGui::BeginDisabled((mSceneManager.GetSceneState() & Scenes::SceneState::PREFAB_EDITOR) || mSceneManager.NoSceneSelected());
        if (sceneStopped) {
          ImGui::BeginDisabled(sceneStopped);
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.5f));
        }
        else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.f, 0.f, 0.7f));
        }

        if (ImGui::Button("Stop")) {
          mSceneManager.StopScene();
        }

        if (sceneStopped) {
          ImGui::EndDisabled();
          ImGui::PopStyleColor();
        }
        else {
          ImGui::PopStyleColor(2);
        }

        // play / pause button
        if (mSceneManager.GetSceneState() & Scenes::SceneState::PLAYING)
        {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.5f));
          if (ImGui::Button("Pause")) {
            mSceneManager.PauseScene();
          }
          ImGui::PopStyleColor();
        }
        else
        {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.6f, 0.f, 1.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.5f, 0.f, 0.7f));
          if (ImGui::Button("Play")) {
            mSceneManager.PlayScene();
          }
          ImGui::PopStyleColor(2);
        }        

        // step button
        ImGui::BeginDisabled(sceneNotPlaying);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Step")) {

        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
        ImGui::EndDisabled();

        ImGui::EndMenuBar();
      }
    }
    ImGui::End();

    if (!sceneStopped) {
      ImGui::PopStyleColor();
    }
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

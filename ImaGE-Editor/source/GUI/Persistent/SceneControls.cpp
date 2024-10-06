/*!*********************************************************************
\file   SceneControls.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the secondary menu bar
        of the editor. Calls upon the SceneManager to start/pause/stop
        the current scene using the controls displayed.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "SceneControls.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar
#include <Events/EventManager.h>
#include <GUI/Styles/FontAwesome6Icons.h>

namespace GUI
{

  SceneControls::SceneControls(const char* name)
    : mSceneManager{ Scenes::SceneManager::GetInstance() }, GUIWindow(name) {}

  void SceneControls::Run()
  {
    static constexpr ImVec4 defBtnBg{ 0.f, 0.f, 0.f, 0.5f }, greenBg{ 0.f, 0.6f, 0.f, 1.f };
    bool const sceneStopped{ !mSceneManager.IsScenePlaying() };

    if (!sceneStopped) {
      ImGui::PushStyleColor(ImGuiCol_MenuBarBg, greenBg);
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
        ImGui::PushStyleColor(ImGuiCol_Button, defBtnBg);
        if (sceneStopped) {
          ImGui::BeginDisabled(sceneStopped);
        }
        else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.f, 0.f, 0.7f));
        }

        if (ImGui::Button(ICON_FA_STOP)) {
          mSceneManager.StopScene();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
          ImGui::SetTooltip("Stop");
        }

        if (sceneStopped) {
          ImGui::EndDisabled();
        }
        else {
          ImGui::PopStyleColor(2);
        }

        // play / pause button
        if (mSceneManager.GetSceneState() & Scenes::SceneState::PLAYING)
        {
          if (ImGui::Button(ICON_FA_PAUSE)) {
            mSceneManager.PauseScene();
          }
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Pause");
          }
        }
        else
        {
          ImGui::PushStyleColor(ImGuiCol_Button, sceneStopped ? ImVec4(0.f, 0.6f, 0.f, 1.f) : defBtnBg);
          if (sceneStopped) {
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.5f, 0.f, 0.7f));
          }
          if (ImGui::Button(ICON_FA_PLAY)) {
            mSceneManager.PlayScene();
          }
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Play");
          }
          ImGui::PopStyleColor(sceneStopped ? 2 : 1);
        }        

        // step button
        ImGui::BeginDisabled(sceneNotPlaying);
        if (ImGui::Button(ICON_FA_STAIRS)) {

        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
          ImGui::SetTooltip("Step");
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
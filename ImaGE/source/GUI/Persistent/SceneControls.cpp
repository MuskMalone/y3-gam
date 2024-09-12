#include <pch.h>
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
    if (mSceneManager.GetSceneState() == Scenes::SceneState::PLAYING)
    {
      ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.6f, 0.f, 1.f));
    }

    if (ImGui::BeginViewportSideBar("##SceneControls", (ImGuiViewportP*)(void*)ImGui::GetMainViewport(),
      ImGuiDir_Up, ImGui::GetFrameHeight(), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
    {
      if (ImGui::BeginMenuBar())
      {
        static float const xOffset{ (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("     PlayPauseStep").x) * 0.5f };
        ImGui::SetCursorPosX(xOffset);

        // stop button
        bool const sceneStopped{ mSceneManager.GetSceneState() == Scenes::SceneState::STOPPED },
          sceneNotPlaying{ mSceneManager.GetSceneState() != Scenes::SceneState::PLAYING };
        if (sceneStopped) {
          ImGui::BeginDisabled();
        }
        else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.f, 0.f, 0.7f));
        }

        if (ImGui::Button("Stop")) {
          QUEUE_EVENT(Events::StopSceneEvent);
        }

        if (sceneStopped) {
          ImGui::EndDisabled();
        }
        else {
          ImGui::PopStyleColor();
          ImGui::PopStyleColor();
        }

        // play / pause button
        if (mSceneManager.GetSceneState() == Scenes::SceneState::PLAYING)
        {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.5f));
          if (ImGui::Button("Pause")) {
            QUEUE_EVENT(Events::PauseSceneEvent);
          }
          ImGui::PopStyleColor();
        }
        else
        {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.6f, 0.f, 1.f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.5f, 0.f, 0.7f));
          if (ImGui::Button("Play")) {
            QUEUE_EVENT(Events::StartSceneEvent);
          }
          ImGui::PopStyleColor();
          ImGui::PopStyleColor();
        }        

        // step button
        if (sceneNotPlaying) { ImGui::BeginDisabled(); }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Step")) {

        }
        ImGui::PopStyleColor();

        if (sceneNotPlaying) { ImGui::EndDisabled(); }

        ImGui::EndMenuBar();
      }
    }
    ImGui::End();

    if (mSceneManager.GetSceneState() == Scenes::SceneState::PLAYING)
    {
      ImGui::PopStyleColor();
    }
  }

} // namespace GUI

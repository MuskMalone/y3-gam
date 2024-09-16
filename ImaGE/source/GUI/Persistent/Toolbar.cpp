#include <pch.h>
#ifndef IMGUI_DISABLE
#include "Toolbar.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <filesystem>

namespace GUI
{

  Toolbar::Toolbar(std::string const& name, std::vector<std::unique_ptr<GUIWindow>> const& windowsRef) :
    mWindowsRef{ windowsRef }, mSceneManager{ Scenes::SceneManager::GetInstance() },
    mScenePopup{ false }, mPrefabPopup{ false }, GUIWindow(name) {}

  void Toolbar::Run()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        const char* const sceneFilter{ "Scenes (*.scn)\0*.scn" }, * const initialDir{ ".\\Assets\\Scenes" };
        bool const noSceneSelected{ mSceneManager.NoSceneSelected() }
          , lockControls{ mSceneManager.IsScenePlaying() || mSceneManager.GetSceneState() == Scenes::SceneState::PREFAB_EDITOR };

        // im sorry this is messy
        // i need to disable different stuff based on the scene state
        if (lockControls) {
          ImGui::BeginDisabled();
        }

        if (ImGui::MenuItem("New Scene")) {
          mScenePopup = true;
        }

        if (noSceneSelected) {
          ImGui::BeginDisabled();
        }

        if (ImGui::MenuItem("New Prefab")) {
          mPrefabPopup = true;
        }

        if (ImGui::MenuItem("Save Scene")) {
          mSceneManager.SaveScene();
        }

        if (noSceneSelected) {
          ImGui::EndDisabled();
        }

        if (ImGui::MenuItem("Load Scene")) {
          std::string const scenePath{ AssetHelpers::LoadFileFromExplorer(sceneFilter, 1, initialDir) };

          if (!scenePath.empty()) {
            QUEUE_EVENT(Events::LoadSceneEvent, std::filesystem::path(scenePath).stem().string(), scenePath);
          }
        }

        if (lockControls) {
          ImGui::EndDisabled();
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View"))
      {
        for (auto const& window : mWindowsRef)
        {
          if (ImGui::MenuItem(window->GetName().c_str(), nullptr, window->IsActive()))
          {
            window->Toggle();
          }
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Options"))
      {

        ImGui::EndMenu();
      }

      // update popups
      if (mScenePopup) {
        ImGui::OpenPopup("Create New Scene");
        mScenePopup = false;
      }
      else if (mPrefabPopup) {
        ImGui::OpenPopup("Create New Prefab");
        mPrefabPopup = false;
      }
      RunNewScenePopup();
      RunNewPrefabPopup();

      ImGui::EndMainMenuBar();
    }
  }

  void Toolbar::RunNewScenePopup()
  {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      static std::string sceneName{};
      static bool blankWarning{ false }, existingSceneWarning{ false };

      if (blankWarning)
      {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Name cannot be blank!!!");
      }
      else if (existingSceneWarning)
      {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Scene already exists!");
      }

      ImGui::Text("Name of Scene:");
      ImGui::SameLine();
      if (ImGui::InputText(".scn", &sceneName))
      {
        blankWarning = existingSceneWarning = false;
      }

      ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Cancel Create ").x));
      if (ImGui::Button("Cancel"))
      {
        sceneName.clear();
        blankWarning = existingSceneWarning = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Create"))
      {
        // if name is blank / whitespace, reject it
        if (sceneName.find_first_not_of(" ") == std::string::npos)
        {
          blankWarning = true;
          existingSceneWarning = false;
        }
        // @TODO: ADD CHECK WHEN ASSET MANAGER IS UP
        /*else if (Assets::AssetManager::GetInstance().HasScene(sceneName))
        {
          existingSceneWarning = true;
          blankWarning = false;
        }*/
        else
        {
          QUEUE_EVENT(Events::LoadSceneEvent, sceneName, std::string());
          blankWarning = existingSceneWarning = false;
          sceneName.clear();
          ImGui::CloseCurrentPopup();
        }
      }

      ImGui::EndPopup();
    }
  }

  void Toolbar::RunNewPrefabPopup()
  {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create New Prefab", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      static std::string prefabName{};
      static bool blankWarning{ false }, existingPrefabWarning{ false };

      if (blankWarning)
      {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Name cannot be blank!!!");
      }
      else if (existingPrefabWarning)
      {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Prefab already exists!");
      }

      ImGui::Text("Name of Prefab:");
      ImGui::SameLine();
      if (ImGui::InputText("##PrefabNameInput", &prefabName))
      {
        blankWarning = existingPrefabWarning = false;
      }

      ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Cancel Create ").x));
      if (ImGui::Button("Cancel"))
      {
        prefabName.clear();
        blankWarning = existingPrefabWarning = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Create"))
      {
        // if name is blank / whitespace, reject it
        if (prefabName.find_first_not_of(" ") == std::string::npos)
        {
          blankWarning = true;
          existingPrefabWarning = false;
        }
        // @TODO: ADD CHECK WHEN PREFAB MANAGER IS UP
        /*else if (Prefabs::PrefabManager::GetInstance().DoesPrefabExist(prefabName))
        {
          existingPrefabWarning = true;
          blankWarning = false;
        }*/
        else
        {
          QUEUE_EVENT(Events::EditPrefabEvent, prefabName, "");
          blankWarning = existingPrefabWarning = false;;
          prefabName.clear();
          ImGui::CloseCurrentPopup();
        }
      }

      ImGui::EndPopup();
    }
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

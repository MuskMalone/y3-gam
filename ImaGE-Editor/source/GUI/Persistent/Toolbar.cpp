/*!*********************************************************************
\file   Toolbar.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the main menubar of the
        editor. Contains options related to files and editor settings.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Toolbar.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Events/EventManager.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <filesystem>
#include <Prefabs/PrefabManager.h>
#include "GUI/GUIVault.h"

namespace GUI
{

  Toolbar::Toolbar(const char* name, std::vector<std::shared_ptr<GUIWindow>> const& windowsRef) : GUIWindow(name),
    mWindowsRef{ windowsRef }, mScenePopup{ false }, mPrefabPopup{ false },
    mDisableAll{ false }, mAllowCreationOnly{ true }
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &Toolbar::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &Toolbar::HandleEvent, this);
  }

  void Toolbar::Run()
  {

    if (ImGui::BeginMainMenuBar())
    {
      float const contentRegionX{ ImGui::GetContentRegionAvail().x };

      if (ImGui::BeginMenu("File"))
      {
        const char* const sceneFilter{ "Scenes (*.scn)\0*.scn" }, * const initialDir{ "..\\Assets\\Scenes" };
        bool const creationMode{ !mDisableAll && mAllowCreationOnly };

        // im sorry this is messy
        // i need to disable different stuff based on the scene state
        if (mDisableAll) {
          ImGui::BeginDisabled();
        }

        if (ImGui::MenuItem("New Scene")) {
          mScenePopup = true;
        }

        if (ImGui::MenuItem("New Prefab")) {
          mPrefabPopup = true;
        }

        if (creationMode) {
          ImGui::BeginDisabled();
        }

        if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
          QUEUE_EVENT(Events::SaveSceneEvent, GUIVault::sSerializePrettyScene);
        }

        if (creationMode) {
          ImGui::EndDisabled();
        }

        if (ImGui::MenuItem("Load Scene")) {
          std::string const scenePath{ AssetHelpers::LoadFileFromExplorer(sceneFilter, 1, initialDir) };

          if (!scenePath.empty()) {
            QUEUE_EVENT(Events::LoadSceneEvent, std::filesystem::path(scenePath).stem().string(), scenePath);
          }
        }

        if (mDisableAll) {
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
        if (ImGui::BeginMenu("Theme")) {
          GUI::Styler& styler{ GUIVault::GetStyler() };
          for (GUI::CustomTheme i{}; i < GUI::CustomTheme::NUM_ITEMS; ++i) {
            bool const currentlyActive{ (i == styler.GetCurrentTheme()) ? true : false };

            if (ImGui::MenuItem(styler.GetCustomThemeString(i).c_str(), nullptr, currentlyActive)) {
              styler.SetCurrentTheme(i);
            }
          }

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Serialization Format")) {
          if (ImGui::BeginMenu("Scenes")) {
            if (ImGui::MenuItem("PRETTY", nullptr, GUIVault::sSerializePrettyScene)) {
              GUIVault::sSerializePrettyScene = true;
            }

            if (ImGui::MenuItem("COMPACT", nullptr, !GUIVault::sSerializePrettyScene)) {
              GUIVault::sSerializePrettyScene = false;
            }

            ImGui::EndMenu();
          }

          if (ImGui::BeginMenu("Prefabs")) {
            if (ImGui::MenuItem("PRETTY", nullptr, GUIVault::sSerializePrettyPrefab)) {
              GUIVault::sSerializePrettyPrefab = true;
            }

            if (ImGui::MenuItem("COMPACT", nullptr, !GUIVault::sSerializePrettyPrefab)) {
              GUIVault::sSerializePrettyPrefab = false;
            }

            ImGui::EndMenu();
          }

          ImGui::EndMenu();
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Debug")) {
        ImGui::MenuItem("Cull out-of-frustum Entities", nullptr, &GUIVault::sShowCulledEntities);
        if (ImGui::IsItemHovered()) {
          ImGui::SetTooltip("Cull all entities outside the frustum in the editor view");
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Help")) {
        if (ImGui::BeginMenu("Controls")) {
          if (ImGui::BeginTable("ControlsTable", 3, ImGuiTableFlags_None)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("CAMERA");
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(1); ImGui::Text("Left Click:");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Select Entity");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Middle Click:");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Pan");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Scroll:");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Zoom");

            ImGui::TableNextRow(); ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("While Right-click Held:");
            ImGui::TableSetColumnIndex(2); ImGui::Text("");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Left Click:");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Look");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("WASD QE:");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Move");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("GUIZMOS:");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Translate: ");
            ImGui::TableSetColumnIndex(2); ImGui::Text("T");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Scale: ");
            ImGui::TableSetColumnIndex(2); ImGui::Text("S");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Rotate: ");
            ImGui::TableSetColumnIndex(2); ImGui::Text("R");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Scene:");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Play/Stop Scene: ");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Ctrl + P");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Free/Lock Cursor: ");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Ctrl + O");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("Debug Lines: ");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Ctrl + D");

            ImGui::EndTable();
          }

          ImGui::EndMenu();
        }

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

      if (GUI::GUIVault::sDevTools) {
        float const originalCursorX{ ImGui::GetCursorPosX() };
        ImGui::SetCursorPosX(contentRegionX * 0.5f - ImGui::CalcTextSize("DEVELOP").x);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 200));
        ImGui::Text("DEVELOPER MODE");
        ImGui::PopStyleColor();
        ImGui::SetCursorPosX(originalCursorX);
      }

      ImGui::EndMainMenuBar();
    }
  }

  EVENT_CALLBACK_DEF(Toolbar, HandleEvent)
  {
    switch (event->GetCategory())
    {
    case Events::EventType::EDIT_PREFAB:
      mDisableAll = true;
      break;
    case Events::EventType::SCENE_STATE_CHANGE:
    {
      switch (CAST_TO_EVENT(Events::SceneStateChange)->mNewState)
      {
      case Events::SceneStateChange::STOPPED:
        mAllowCreationOnly = true;
        mDisableAll = false;
        break;
      case Events::SceneStateChange::NEW:
      case Events::SceneStateChange::CHANGED:
        mAllowCreationOnly = mDisableAll = false;
        break;
      case Events::SceneStateChange::STARTED:
      case Events::SceneStateChange::PAUSED:
        mDisableAll = true;
        break;
      default: break;
      }
    }
    default: break;
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
      if (!ImGui::IsAnyItemActive()) ImGui::SetKeyboardFocusHere();
      if (ImGui::InputText(".scn", &sceneName)) {
        blankWarning = existingSceneWarning = false;
      }

      ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Cancel Create ").x));
      if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        sceneName.clear();
        blankWarning = existingSceneWarning = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Create") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        // if name is blank / whitespace, reject it
        if (sceneName.find_first_not_of(" ") == std::string::npos) {
          blankWarning = true;
          existingSceneWarning = false;
        }
        // @TODO: ADD CHECK WHEN ASSET MANAGER IS UP
        /*else if (Assets::AssetManager::GetInstance().HasScene(sceneName))
        {
          existingSceneWarning = true;
          blankWarning = false;
        }*/
        else {
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
      static std::string input{};
      static bool blankWarning{ false }, existingPrefabWarning{ false };
      static auto& prefabMan{ Prefabs::PrefabManager::GetInstance() };

      if (blankWarning) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Name cannot be blank!!!");
      }
      else if (existingPrefabWarning) {
        ImGui::TextColored(ImVec4(0.99f, 0.82f, 0.09f, 1.0f), "Warning: Prefab already exists.");
        ImGui::TextColored(ImVec4(0.99f, 0.82f, 0.09f, 1.0f), "File will be overwritten!!");
      }

      ImGui::Text("Name of Prefab:");
      ImGui::SameLine();
      if (!ImGui::IsAnyItemActive()) ImGui::SetKeyboardFocusHere();
      if (ImGui::InputText("##PrefabNameInput", &input)) {
        existingPrefabWarning = prefabMan.DoesPrefabExist(input);
        blankWarning = false;
      }

      ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Cancel Create ").x));
      if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        input.clear();
        blankWarning = existingPrefabWarning = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Create") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        // if name is blank / whitespace, reject it
        if (input.find_first_not_of(" ") == std::string::npos) {
          blankWarning = true;
          existingPrefabWarning = false;
        }
        else {
          QUEUE_EVENT(Events::EditPrefabEvent, input, std::string());
          blankWarning = existingPrefabWarning = false;
          input.clear();
          ImGui::CloseCurrentPopup();
        }
      }

      ImGui::EndPopup();
    }
  }

} // namespace GUI

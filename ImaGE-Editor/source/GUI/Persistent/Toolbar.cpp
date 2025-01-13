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
#include "GUI/GUIVault.h"
#include <GUI/Helpers/ImGuiHelpers.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <Core/Components/Light.h>
#include <Asset/IGEAssets.h>
#include <Graphics/Mesh/IMSH.h>

#include <Serialization/Serializer.h>
#include <Events/EventManager.h>
#include <Prefabs/PrefabManager.h>
#include "Scenes/SceneManager.h"
#include <Graphics/Renderer.h>

#include <filesystem>
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace {
  void NextRowTable(const char* labelName, float inputSize);
}

namespace GUI
{

  Toolbar::Toolbar(const char* name, std::vector<std::shared_ptr<GUIWindow>> const& windowsRef,
    std::weak_ptr<Graphics::EditorCamera> editorCam) : GUIWindow(name),
      mWindowsRef{ windowsRef }, mEditorCam{ std::move(editorCam) }, mScenePopup{false}, mPrefabPopup{false},
      mDisableAll{ false }, mAllowCreationOnly{ true }
  {
    SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &Toolbar::OnSceneStateChange, this);
    SUBSCRIBE_CLASS_FUNC(Events::EditPrefabEvent, &Toolbar::OnPrefabEdit, this);
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

        ImGui::EndMenu(); // File
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

        ImGui::EndMenu(); // View
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

          ImGui::EndMenu(); // Theme
        }

        SerializationMenu("Serialization Format");

        ImGui::EndMenu(); // Options
      }

      bool const noScene{ Scenes::SceneManager::GetInstance().NoSceneSelected() };

      if (ImGui::BeginMenu("Debug")) {

        if (EditorCamMenu("Editor Camera", noScene)) {
          // let editor know the scene has been modified (to trigger star symbol)
          QUEUE_EVENT(Events::SceneModifiedEvent);
        }

        ImGui::MenuItem("Cull out-of-frustum Entities", nullptr, &GUIVault::sShowCulledEntities);
        if (ImGui::IsItemHovered()) {
          ImGui::SetTooltip("Cull all entities outside the frustum in the editor view");
        }

        ResavePrefabsMenuItem("Re-save all Prefabs");

        ReimportMeshesMenuItem("Re-import all Meshes");

        ImGui::EndMenu(); // Debug
      }

      HelpMenu("Help");

      if (LightingMenu("Lighting", noScene)) {
        // let editor know the scene has been modified (to trigger star symbol)
        QUEUE_EVENT(Events::SceneModifiedEvent);
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

  EVENT_CALLBACK_DEF(Toolbar, OnSceneStateChange)
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

  EVENT_CALLBACK_DEF(Toolbar, OnPrefabEdit) {
    mDisableAll = true;
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

  bool Toolbar::LightingMenu(const char* label, bool disabled) const {
    // early return if menu not open
    if (!ImGui::BeginMenu(label)) { return false; }

    bool modified{ false };

    ImGui::BeginDisabled(disabled);

    if (ImGui::BeginTable("##LightTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      bool modified{ false };
      Component::LightGlobalProps& globalProps{ Component::Light::sGlobalProps };

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, 175);
      ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, 200);
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient Color");
      ImGui::TableSetColumnIndex(1);
      if (ImGui::ColorEdit4("##Ambc", &globalProps.ambColor[0], ImGuiColorEditFlags_NoAlpha)) {
        modified = true;
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient Intensity: ");
      ImGui::TableSetColumnIndex(1);
      if (ImGui::DragFloat("##AmbIn", &globalProps.ambIntensity, 0.001f, 0.f, FLT_MAX, "%.3f")) {
        modified = true;
      }

      ImGui::EndTable();
    }

    ImGui::EndDisabled();
    ImGui::EndMenu();

    return modified;
  }

  bool Toolbar::EditorCamMenu(const char* label, bool disabled) const {
    std::shared_ptr<Graphics::EditorCamera> eCam{ mEditorCam.lock() };
    if (!eCam) {
      IGE_DBGLOGGER.LogError("Editor Camera doesn't exist!");
      return false;
    }
    // early return if menu not open
    else if (!ImGui::BeginMenu(label)) {
      return false;
    }
    ImGui::BeginDisabled(disabled);

    bool modified{ false };
    float const col1{ 80.f }, elemWidth{ 200.f };

    if (ImGui::BeginTable("EditorCamVec3Table", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      float const vec3Width{ elemWidth * 0.33f };
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, col1);
      ImGui::TableSetupColumn(" X", ImGuiTableColumnFlags_WidthFixed, vec3Width);
      ImGui::TableSetupColumn(" Y", ImGuiTableColumnFlags_WidthFixed, vec3Width);
      ImGui::TableSetupColumn(" Z", ImGuiTableColumnFlags_WidthFixed, vec3Width);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputFloat3("Position", &eCam->mPosition.x, elemWidth, false, -FLT_MAX, FLT_MAX, 0.1f)) {
        modified = true;
      }
      ImGui::EndTable();
    }

    if (ImGui::BeginTable("EditorCamTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, col1);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, elemWidth);

      NextRowTable("Yaw", elemWidth);
      if (ImGui::SliderFloat("##Yaw", &eCam->mYaw, -180.f, 180.f)) {
        modified = true;
      }
      NextRowTable("Pitch", elemWidth);
      if (ImGui::SliderFloat("##Pitch", &eCam->mPitch, -180.f, 180.f)) {
        modified = true;
      }

      NextRowTable("FOV", elemWidth);
      if (ImGui::SliderFloat("##FOV", &eCam->mFov, 0.f, 180.f)) {
        modified = true;
      }

      NextRowTable("Near Clip", elemWidth);
      if (ImGui::DragFloat("##Near", &eCam->mNearClip, 5.f, -100.f, FLT_MAX)) {
        modified = true;
      }
      NextRowTable("Far Clip", elemWidth);
      if (ImGui::DragFloat("##Far", &eCam->mFarClip, 5.f, 0.f, 1000.f)) {
        modified = true;
      }

      ImGui::EndTable();
    }

    if (ImGui::Button("Reset to Defaults")) {
      eCam->InitForEditorView();
      modified = true;
    }

    ImGui::EndDisabled();
    ImGui::EndMenu();

    return modified;
  }

  void Toolbar::ResavePrefabsMenuItem(const char* label) const {
    if (ImGui::MenuItem(label)) {
      IGE::Assets::AssetManager& am{ IGE_ASSETMGR };

      for (auto const& file : std::filesystem::recursive_directory_iterator(gPrefabsDirectory)) {
        if (file.is_directory() || file.path().extension() != gPrefabFileExt) { continue; }

        std::string const filePath{ file.path().string() };
        try {
          IGE::Assets::GUID guid{ am.LoadRef<IGE::Assets::PrefabAsset>(filePath) };
          Serialization::Serializer::SerializePrefab(am.GetAsset<IGE::Assets::PrefabAsset>(guid)->mPrefabData, filePath);
          IGE_DBGLOGGER.LogInfo("Re-saved " + filePath);
        }
        catch (Debug::ExceptionBase&) {
          IGE_DBGLOGGER.LogError("Unable to load " + filePath);
        }
      }
    }
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Automatically saves all prefabs again");
      ImGui::Text("For use when components/formats are modified and require saving to a new format.");
      ImGui::EndTooltip();
    }
  }

  void Toolbar::ReimportMeshesMenuItem(const char* label) const {
    if (ImGui::MenuItem(label)) {
      for (auto const& file : std::filesystem::directory_iterator(std::string(gAssetsDirectory) + "Models\\Compiled")) {
        Graphics::AssetIO::IMSH imsh{};
        std::string const fp{ file.path().string() };

        imsh.ReadFromBinFile(fp);
        imsh.WriteToBinFile(fp);

        IGE_DBGLOGGER.LogInfo("Reimported " + fp);
      }
    }
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Automatically imports all models again");
      ImGui::Text("For use when certain models are outdated and require saving to a new format.");
      ImGui::EndTooltip();
    }
  }

  void Toolbar::SerializationMenu(const char* label) const {
    if (!ImGui::BeginMenu(label)) { return; }

    if (ImGui::BeginMenu("Scenes")) {
      if (ImGui::MenuItem("PRETTY", nullptr, GUIVault::sSerializePrettyScene)) {
        GUIVault::sSerializePrettyScene = true;
      }

      if (ImGui::MenuItem("COMPACT", nullptr, !GUIVault::sSerializePrettyScene)) {
        GUIVault::sSerializePrettyScene = false;
      }

      ImGui::EndMenu(); // Scenes
    }

    if (ImGui::BeginMenu("Prefabs")) {
      if (ImGui::MenuItem("PRETTY", nullptr, GUIVault::sSerializePrettyPrefab)) {
        GUIVault::sSerializePrettyPrefab = true;
      }

      if (ImGui::MenuItem("COMPACT", nullptr, !GUIVault::sSerializePrettyPrefab)) {
        GUIVault::sSerializePrettyPrefab = false;
      }

      ImGui::EndMenu(); // Prefabs
    }

    ImGui::EndMenu(); // label
  }

  void Toolbar::HelpMenu(const char* label) const {
    if (!ImGui::BeginMenu(label)) { return; }

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
      ImGui::EndMenu(); // Controls
    }

    ImGui::EndMenu(); // label
  }
} // namespace GUI

namespace {
  void NextRowTable(const char* labelName, float inputSize) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(labelName);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(inputSize);
  }
}

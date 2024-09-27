#include "Toolbar.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Events/EventManager.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <filesystem>
#include <Prefabs/PrefabManager.h>
#include "GUI/GUIManager.h"

namespace GUI
{

  Toolbar::Toolbar(std::string const& name, std::vector<std::shared_ptr<GUIWindow>> const& windowsRef) : GUIWindow(name),
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
      if (ImGui::BeginMenu("File"))
      {
        const char* const sceneFilter{ "Scenes (*.scn)\0*.scn" }, * const initialDir{ ".\\Assets\\Scenes" };
        bool const creationMode{ !mDisableAll && mAllowCreationOnly };

        // im sorry this is messy
        // i need to disable different stuff based on the scene state
        if (mDisableAll) {
          ImGui::BeginDisabled();
        }

        if (ImGui::MenuItem("New Scene")) {
          mScenePopup = true;
        }

        if (creationMode) {
          ImGui::BeginDisabled();
        }

        if (ImGui::MenuItem("New Prefab")) {
          mPrefabPopup = true;
        }

        if (ImGui::MenuItem("Save Scene (Ctrl+S)")) {
          QUEUE_EVENT(Events::SaveSceneEvent);
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

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Theme")) {
        GUI::Styler& styler{ GUIManager::GetStyler() };
        for (GUI::CustomTheme i{}; i < GUI::CustomTheme::NUM_ITEMS; ++i) {
          bool const currentlyActive{ (i == styler.GetCurrentTheme()) ? true : false };

          if (ImGui::MenuItem(styler.GetCustomThemeString(i).c_str(), nullptr, currentlyActive)) {
            styler.SetCurrentTheme(i);
          }
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
      if (ImGui::Button("Cancel")) {
        sceneName.clear();
        blankWarning = existingSceneWarning = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Create")) {
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
    if (ImGui::BeginPopupModal("Create Prefab", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
      if (ImGui::Button("Cancel")) {
        input.clear();
        blankWarning = existingPrefabWarning = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Create")) {
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

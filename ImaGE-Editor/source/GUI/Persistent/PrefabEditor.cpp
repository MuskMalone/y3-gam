/*!*********************************************************************
\file   PrefabEditor.cpp
\author chengen.lau\@digipen.edu
\date   14-September-2024
\brief  Contains the class encapsulating functions for the prefab
        editor feature. Different windows in the engine reference
        this class to determine which mode is currently running
        (scene or prefab editor).

        Why is this a persistent element (non-dockable) ?
          - This class tags upon the inspector and scene hierarchy
            during prefab editing mode. It only tracks the changes
            made and updates the PrefabManager accordingly.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include "PrefabEditor.h"
#include <Events/EventManager.h>
#include <Events/InputEvents.h>
#include <ImGui/imgui.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Reflection/ComponentTypes.h>

namespace GUI
{

  PrefabEditor::PrefabEditor(std::string const& name) :
    mPrefabName{}, mPrefabPath{}, mPrefabInstance{},
    mIsEditing{ false }, mEscTriggered{ false }, GUIWindow(name)
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &PrefabEditor::HandleEvent, this);
  }

  void PrefabEditor::Run()
  {
    if (!mIsEditing) { return; }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      if (mEscTriggered) {
        mEscTriggered = false;
      }
      else {
        ImGui::OpenPopup("Return to Scene");
        mEscTriggered = true;
      }
    }
    BackToScenePopup();
  }

  EVENT_CALLBACK_DEF(PrefabEditor, HandleEvent)
  {
    switch (event->GetCategory())
    {
    case Events::EventType::EDIT_PREFAB:
    {
      auto editPrefabEvent{ std::static_pointer_cast<Events::EditPrefabEvent>(event) };
      auto& prefabMan{ Prefabs::PrefabManager().GetInstance() };
      mPrefabName = editPrefabEvent->mPrefab;
      mPrefabPath = editPrefabEvent->mPath;
      prefabMan.LoadPrefab(mPrefabName, mPrefabPath); // force load the prefab first
      mPrefabInstance = prefabMan.SpawnPrefab(mPrefabName);
      mIsEditing = true;
      break;
    }
    }
  }

  void PrefabEditor::BackToScenePopup()
  {
    if (ImGui::BeginPopupModal("Return to Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      if (!mEscTriggered) { ImGui::CloseCurrentPopup(); }

      ImGui::Text("Save changes made to prefab?");
      ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Cancel Discard Changes Save ").x));

      if (ImGui::Button("Cancel"))
      {
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.f));
      if (ImGui::Button("Discard Changes"))
      {
        Scenes::SceneManager::GetInstance().StopScene();

        ResetPrefabEditor();
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.65f, 0.f, 1.f));
      if (ImGui::Button("Save"))
      {
        Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };

        if (mPrefabPath.empty())
        {
          pm.CreatePrefabFromEntity(mPrefabInstance, mPrefabName);
        }
        else
        {
          Debug::DebugLogger::GetInstance().LogInfo("[PrefabEditor] Saved " + mPrefabName);
          pm.UpdatePrefabFromEditor(mPrefabInstance, mRemovedChildren, mRemovedComponents, mPrefabPath);
          pm.ClearMappings();
        }
        Scenes::SceneManager::GetInstance().StopScene();

        ResetPrefabEditor();
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::EndPopup();
    }
  }

  void PrefabEditor::ResetPrefabEditor()
  {
    mPrefabInstance = ECS::Entity();
    mPrefabName.clear();
    mPrefabPath.clear();
    mIsEditing = mEscTriggered = false;
    mRemovedChildren.clear();
    mRemovedComponents.clear();
  }

} // namespace GUI

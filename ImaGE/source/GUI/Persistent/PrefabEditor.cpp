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
#include <pch.h>
#ifndef IMGUI_DISABLE
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
      mPrefabName = editPrefabEvent->mPrefab;
      mIsEditing = true;
      // if new prefab, create an entity with the prefab's name
      if (editPrefabEvent->mPath.empty()) {
        mPrefabInstance.first = ECS::EntityManager::GetInstance().CreateEntity();
        mPrefabInstance.first.GetComponent<Component::Tag>().tag = mPrefabName;
        return;
      }

      auto& prefabMan{ Prefabs::PrefabManager().GetInstance() };
      prefabMan.LoadPrefab(mPrefabName); // force load the prefab first
      mPrefabInstance = prefabMan.SpawnPrefabAndMap(mPrefabName);
      mPrefabPath = editPrefabEvent->mPath;
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
        ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
        if (!mPrefabInstance.first) { // should also check if its valid
          // if the current prefabInstance isnt valid,
          // search for the first entity without a parent
          for (auto const& e : entityMan.GetAllEntities()) {
            if (!entityMan.HasParent(e)) {
              mPrefabInstance.first = e;
              break;
            }
          }
        }
        
        Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };

        if (mPrefabPath.empty() || mPrefabInstance.second.Empty()) {
          pm.CreatePrefabFromEntity(mPrefabInstance.first, mPrefabName);
        }
        else
        {
          Debug::DebugLogger::GetInstance().LogInfo("[PrefabEditor] Saved " + mPrefabName);
          pm.UpdatePrefabFromEditor(mPrefabInstance.first, mPrefabName, mPrefabInstance.second, mPrefabPath);
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
    mPrefabInstance = {};
    mPrefabName.clear();
    mPrefabPath.clear();
    mIsEditing = mEscTriggered = false;
    mRemovedChildren.clear();
    mRemovedComponents.clear();
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

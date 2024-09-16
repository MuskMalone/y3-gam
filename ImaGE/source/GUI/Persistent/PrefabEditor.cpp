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

namespace GUI
{

  PrefabEditor::PrefabEditor(std::string const& name) :
    mPrefabName{}, mPrefabPath{}, mPrefabInstance{},
    mIsEditing{ false }, mEscTriggered{ false }, GUIWindow(name)
  {
    //SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &PrefabEditor::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::KEY_TRIGGERED, &PrefabEditor::HandleEvent, this);
  }

  void PrefabEditor::Run()
  {
    if (mEscTriggered) {
      ImGui::OpenPopup("Return to Scene");
      mEscTriggered = false;
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
      mIsEditing = true;
      break;
    }
    case Events::EventType::KEY_TRIGGERED:
    {
      if (!mIsEditing) { return; }

      auto keyTriggeredEvent{ std::static_pointer_cast<Events::EditPrefabEvent>(event) };
      // check for ESC key
      break;
    }
    }
  }

  void PrefabEditor::BackToScenePopup()
  {
    if (ImGui::BeginPopupModal("Return to Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
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
        QUEUE_EVENT(Events::StopSceneEvent);

        ResetPrefabEditor();
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.65f, 0.f, 1.f));
      if (ImGui::Button("Save"))
      {
//        Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };
//
//        if (m_newPrefab)
//        {
//          pm.CreatePrefabFromEntity(m_prefabInstance, m_prefabName);
//        }
//        else
//        {
//          //CheckForDeletions();
//#ifdef PREFAB_EDITOR_DEBUG
//          std::cout << "Saving...\n";
//#endif
//          //pm.UpdatePrefabFromEditor(m_prefabInstance, mRemovedChildren, mRemovedComponents, m_prefabPath);
//        }
        QUEUE_EVENT(Events::StopSceneEvent);

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
    mIsEditing = false;
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

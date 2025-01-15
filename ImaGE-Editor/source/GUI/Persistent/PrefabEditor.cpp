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
#include "PrefabEditor.h"
#include <Events/EventManager.h>
#include <Events/InputEvents.h>
#include <ImGui/imgui.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Reflection/ComponentTypes.h>
#include <GUI/GUIVault.h>

namespace GUI
{

  PrefabEditor::PrefabEditor(const char* name) :
    mPrefabName{}, mGUID{}, mPrefabInstance{},
    mIsEditing{ false }, mEscTriggered{ false }, GUIWindow(name)
  {
    SUBSCRIBE_CLASS_FUNC(Events::EditPrefabEvent, &PrefabEditor::OnPrefabEdit, this);
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

  EVENT_CALLBACK_DEF(PrefabEditor, OnPrefabEdit)
  {
    auto editPrefabEvent{ std::static_pointer_cast<Events::EditPrefabEvent>(event) };
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };

    mPrefabName = editPrefabEvent->mPrefab;
    mIsEditing = true;
    // if new prefab, create an entity with the prefab's name
    if (editPrefabEvent->mPath.empty()) {
      mPrefabInstance.first = ECS::EntityManager::GetInstance().CreateEntity();
      mPrefabInstance.first.GetComponent<Component::Tag>().tag = mPrefabName;
      return;
    }

    try {
      mGUID = am.LoadRef<IGE::Assets::PrefabAsset>(editPrefabEvent->mPath);
      mPrefabInstance = Prefabs::PrefabManager::GetInstance().SpawnPrefabAndMap(mGUID);
    }
    catch (Debug::ExceptionBase const&) {
      IGE_DBGLOGGER.LogInfo("Untracked file detected. Registering to Asset Manager...");
      std::string const filepath{ gPrefabsDirectory + mPrefabName + gPrefabFileExt };
      am.ImportAsset<IGE::Assets::PrefabAsset>(filepath);
      IGE::Assets::GUID const guid{ am.LoadRef<IGE::Assets::PrefabAsset>(filepath) };
      mPrefabInstance = Prefabs::PrefabManager::GetInstance().SpawnPrefabAndMap(guid);
      mGUID = {};
    }
  }

  void PrefabEditor::BackToScenePopup()
  {
    if (ImGui::BeginPopupModal("Return to Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      if (!mEscTriggered) { ImGui::CloseCurrentPopup(); }

      ImGui::Text("Save changes made to prefab?");
      ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Cancel Discard Changes Save ").x));

      if (ImGui::Button("Cancel")) {
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
      if (ImGui::Button("Save") || ImGui::IsKeyPressed(ImGuiKey_Enter))
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

        if (!mGUID.IsValid() || mPrefabInstance.second.Empty()) {
          pm.CreatePrefabFromEntity(mPrefabInstance.first, mPrefabName);
        }
        else
        {
          Debug::DebugLogger::GetInstance().LogInfo("[PrefabEditor] Saved " + mPrefabName);
          pm.UpdatePrefabFromEditor(mPrefabInstance.first, mPrefabName,
            mPrefabInstance.second, mGUID, GUIVault::sSerializePrettyPrefab);
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
    mGUID = {};
    mIsEditing = mEscTriggered = false;
    mRemovedChildren.clear();
    mRemovedComponents.clear();
  }

} // namespace GUI

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
          //CheckForDeletions();
#ifdef _DEBUG
          std::cout << "[PrefabEditor] Saving...\n";
#endif
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

  void PrefabEditor::CheckForDeletions()
  {
    // if its a new prefab, ignore
    if (mPrefabPath.empty()) { return; }

#ifdef _DEBUG
    //std::cout << "[PrefabEditor] Checking for deleted objects...\n";
#endif
    Reflection::ObjectFactory const& of{ Reflection::ObjectFactory::GetInstance() };
    Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };
    Prefabs::VariantPrefab const& ref{ pm.GetVariantPrefab(mPrefabName) };

    // check base entity first
    for (rttr::variant const& comp : ref.mComponents)
    {
      rttr::type const compType{ comp.get_type().get_wrapped_type().get_raw_type() };
      auto iter{ std::find(Reflection::gComponentTypes.cbegin(), Reflection::gComponentTypes.cend(), compType) };
      if (iter == Reflection::gComponentTypes.cend())
      {
        // replace with logger
#ifdef _DEBUG
        std::cout << "Unable to find " + compType.get_name().to_string() + " component type in Reflection::gComponentTypes\n";
#endif
        continue;
      }

      // if entity no longer contains component, means it was removed
      if (!of.GetEntityComponent(mPrefabInstance, *iter).is_valid())
      {
#ifdef _DEBUG
        std::cout << "  Added <0, " << compType.get_name().to_string() << "> to mRemovedComponents\n";
#endif
        mRemovedComponents.emplace_back(Prefabs::PrefabSubData::BasePrefabId, compType);
      } 
    }

    // now check its children and their components
    auto const mappings{ pm.GetEntityPrefab(mPrefabInstance)->get().mObjToEntity };
    for (auto const& subData : ref.mObjects)
    {
      // if child was removed, skip
      if (std::find(mRemovedChildren.cbegin(), mRemovedChildren.cend(), subData.mId) != mRemovedChildren.cend())
      {
#ifdef _DEBUG
        std::cout << "  Skipping id " << subData.mId << " since it was removed\n";
#endif
        continue;
      }

      // else continue to check components
      ECS::Entity const& currEntity{ mappings.at(subData.mId) };
      for (rttr::variant const& comp : subData.mComponents)
      {
        rttr::type const compType{ comp.get_type().get_wrapped_type().get_raw_type() };
        auto iter{ std::find(Reflection::gComponentTypes.cbegin(), Reflection::gComponentTypes.cend(), compType) };
        if (iter == Reflection::gComponentTypes.cend())
        {
          // replace with logger
#ifdef _DEBUG
          std::cout << "Unable to find " + compType.get_name().to_string() + " component type in Reflection::gComponentTypes\n";
#endif
          continue;
        }

        // if entity no longer contains component, means it was removed
        if (!of.GetEntityComponent(currEntity, *iter).is_valid())
        {
#ifdef _DEBUG
          std::cout << "  Added <" << subData.mId << ", " << compType.get_name().to_string() << "> to mRemovedComponents\n";
#endif
          mRemovedComponents.emplace_back(subData.mId, compType);
        }
      }
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

#endif  // IMGUI_DISABLE

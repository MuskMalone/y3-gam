  /*!*********************************************************************
  \file   SceneHierarchy.cpp
  \author chengen.lau\@digipen.edu
  \date   5-October-2024
  \brief  Class encapsulating functions to run the scene hierarchy
          window of the editor. Displays the list of entities currently
          in the scene along with their position in the hierarchy.
          Features right-click options as well as parenting of entities.

  Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
  ************************************************************************/
#include <pch.h>
#include "SceneHierarchy.h"
#include <imgui/imgui.h>
#include <Core/Components/Tag.h>
#include <GUI/GUIManager.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Events/EventManager.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Prefabs/PrefabManager.h>
#include <GUI/Dockable/Inspector.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>

namespace GUI
{

  SceneHierarchy::SceneHierarchy(const char* name) : GUIWindow(name),
    mEntityManager{ ECS::EntityManager::GetInstance() },
    mSceneName{},
    mRightClickedEntity{}, mRightClickMenu{ false }, mEntityOptionsMenu{ false },
    mPrefabPopup{ false }, mFirstTimePfbPopup{ true }, mEditingPrefab{ false }, mLockControls{ false }, mSceneModified{ false }
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &SceneHierarchy::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &SceneHierarchy::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_MODIFIED, &SceneHierarchy::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SAVE_SCENE, &SceneHierarchy::HandleEvent, this);
  }

  void SceneHierarchy::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    if (mSceneName.empty())
    {
      ImGui::Text("No Scene Selected");
      ImGui::End();
      return;
    }


    std::string sceneNameSave{ mSceneName };

    if (!mEditingPrefab) {
      // Ctrl + S to save
      if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        QUEUE_EVENT(Events::SaveSceneEvent);
      }

      ImGui::Text(sceneNameSave.c_str());
    }
    else {
      ImGui::Text(("Editing Prefab: " + sceneNameSave).c_str());
      ImGui::PushStyleColor(ImGuiCol_Text, sEntityHighlightCol);
      ImGui::Text("Press ESC to return to scene");
      ImGui::PopStyleColor();
    }

    ImGui::Separator();

    if (ImGuiHelpers::BeginDrapDropTargetWindow(sDragDropPayload))
    {
      ImGuiPayload const* drop{ ImGui::AcceptDragDropPayload(sDragDropPayload) };
      ECS::Entity droppedEntity{ *reinterpret_cast<ECS::Entity*>(drop->Data) };
      if (mEntityManager.HasParent(droppedEntity)) {
        mEntityManager.RemoveParent(droppedEntity);

        // entity unparented - convert to local and recompute matrix
        Component::Transform& trans{ droppedEntity.GetComponent<Component::Transform>() };
        trans.SetLocalToWorld();
        trans.modified = true;

        QUEUE_EVENT(Events::SceneModifiedEvent);
      }

      ImGui::EndDragDropTarget();
    }

    for (auto const& e : mEntityManager.GetAllEntities())
    {
      if (mEntityManager.HasParent(e)) { continue; }

      RecurseDownHierarchy(e);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && GUIManager::GetSelectedEntity() && !mLockControls) {
      ECS::EntityManager::GetInstance().RemoveEntity(GUIManager::GetSelectedEntity());
      GUIManager::SetSelectedEntity(ECS::Entity());
    }

    if (mEntityOptionsMenu) {
      ImGui::OpenPopup("EntityOptions");
      mEntityOptionsMenu = false;
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
      ImGui::OpenPopup("HierarchyOptions");
      mRightClickMenu = false;
    }
    else if (mPrefabPopup) {
      ImGui::OpenPopup("Create Prefab");
      mPrefabPopup = false;
      mFirstTimePfbPopup = true;
    }
    RunRightClickMenu();
    RunEntityOptions();
    RunPrefabPopup();

    ImGui::End();
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, HandleEvent)
  {
    switch (event->GetCategory())
    {
    case Events::EventType::EDIT_PREFAB:
      mSceneName = CAST_TO_EVENT(Events::EditPrefabEvent)->mPrefab;
      mEditingPrefab = true;
      break;
    case Events::EventType::SCENE_STATE_CHANGE:
    {
      auto sceneStateEvent{ CAST_TO_EVENT(Events::SceneStateChange) };
      switch (sceneStateEvent->mNewState)
      {
      case Events::SceneStateChange::NEW:
      case Events::SceneStateChange::CHANGED:
        mSceneName = sceneStateEvent->mSceneName;
        mEditingPrefab = mSceneModified = false;
        break;
      case Events::SceneStateChange::STOPPED:
        mEditingPrefab = false;
        mSceneName.clear();
        break;
      default: break;
      }

      break;
    }
    case Events::EventType::SCENE_MODIFIED:
      if (mSceneModified) { return; }

      mSceneName += " *";
      mSceneModified = true;
      break;
    case Events::EventType::SAVE_SCENE:
      mSceneName.erase(mSceneName.size() - 2);
      break;
    default:break;
    }
  }

  ECS::Entity SceneHierarchy::CreateNewEntity() const
  {
    auto newEntity{ mEntityManager.CreateEntity() };
    newEntity.GetComponent<Component::Tag>().tag = "Entity " + std::to_string(newEntity.GetEntityID());
    return newEntity;
  }

  void SceneHierarchy::RecurseDownHierarchy(ECS::Entity entity)
  {
    static bool editNameMode{ false }, firstEnterEditMode{ true };
    bool const isCurrentEntity{ GUIManager::GetSelectedEntity() == entity }, isEditMode{ isCurrentEntity && editNameMode };
    // set the flag accordingly
    ImGuiTreeNodeFlags treeFlag{ ImGuiTreeNodeFlags_SpanFullWidth };
    bool const hasChildren{ mEntityManager.HasChild(entity) };
    treeFlag |= hasChildren ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen
      : ImGuiTreeNodeFlags_Leaf;

    if (isCurrentEntity) { treeFlag |= ImGuiTreeNodeFlags_Selected; }

    // create the tree nodes
    std::string entityName{ entity.GetComponent<Component::Tag>().tag };
    std::string const displayName{ isEditMode ? "##" : "" + entityName };

    // highlight if its a prefab instance
    bool const isPrefabInstance{ entity.HasComponent<Component::PrefabOverrides>() };
    if (isPrefabInstance) {
      ImGui::PushStyleColor(ImGuiCol_Text, sEntityHighlightCol);
    }

    if (ImGui::TreeNodeEx((displayName + "##" + std::to_string(entity.GetEntityID())).c_str(), treeFlag))
    {
      // if renaming entity
      if (isEditMode) {
        ImGui::SetItemAllowOverlap();
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 12.f);
        if (firstEnterEditMode) {
          ImGui::SetKeyboardFocusHere();
          firstEnterEditMode = false;
        }
        if (ImGui::InputText("##EntityRename", &entityName, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
          entity.GetComponent<Component::Tag>().tag = entityName;
          editNameMode = mLockControls = false;
          firstEnterEditMode = true;
          QUEUE_EVENT(Events::SceneModifiedEvent);
        }
        ImGui::PopStyleVar();

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
          ImGui::SetWindowFocus(NULL);
          editNameMode = mLockControls = false;
          firstEnterEditMode = true;
        }
      }

      ProcessInput(entity, editNameMode);

      if (hasChildren) {
        for (auto const& child : mEntityManager.GetChildEntity(entity)) {
          RecurseDownHierarchy(child);
        }
      }

      ImGui::TreePop();
    }

    if (isPrefabInstance) {
      ImGui::PopStyleColor();
    }
  }

  void SceneHierarchy::ProcessInput(ECS::Entity entity, bool& editNameMode) {

    bool dragNDropped{ false };
    if (!mLockControls) {
      if (ImGui::BeginDragDropSource())
      {
        ECS::EntityManager::EntityID id{ entity.GetRawEnttEntityID() };
        ImGui::SetDragDropPayload(sDragDropPayload, &id, sizeof(ECS::EntityManager::EntityID), ImGuiCond_Once);
        ImGui::Text(entity.GetTag().c_str());
        ImGui::EndDragDropSource();
      }
      if (ImGui::BeginDragDropTarget())
      {
        dragNDropped = true;
        ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(sDragDropPayload);
        if (drop)
        {
          ECS::Entity droppedEntity{ *reinterpret_cast<ECS::EntityManager::EntityID*>(drop->Data) };
          if (mEntityManager.HasParent(droppedEntity)) {
            mEntityManager.RemoveParent(droppedEntity);
          }
          mEntityManager.SetParentEntity(entity, droppedEntity);
          droppedEntity.GetComponent<Component::Transform>().modified = true;
          // entity has new parent, traverse down hierarchy and update transforms
          TransformHelpers::UpdateTransformToNewParent(droppedEntity);
          QUEUE_EVENT(Events::SceneModifiedEvent);
        }
        ImGui::EndDragDropTarget();
      }

      if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        mRightClickedEntity = entity;
        mEntityOptionsMenu = true;
      }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
      if (GUIManager::GetSelectedEntity() == entity) {
        if (!dragNDropped) {
          editNameMode = mLockControls = true;
        }
      }
      else {
        GUIManager::SetSelectedEntity(entity);
        editNameMode = mLockControls = false;
      }
    }
  }

  void SceneHierarchy::RunRightClickMenu() const
  {
    if (ImGui::BeginPopup("HierarchyOptions"))
    {
      if (ImGui::Selectable("Create Entity"))
      {
        CreateNewEntity();
      }

      ImGui::EndPopup();
    }
  }

  void SceneHierarchy::RunEntityOptions()
  {
    if (ImGui::BeginPopup("EntityOptions"))
    {
      if (ImGui::Selectable("Create Entity")) {
        ECS::Entity newEntity{ CreateNewEntity() };
        mEntityManager.SetParentEntity(mRightClickedEntity, newEntity);
      }

      // @TODO: need a way to deep copy components
      //ImGui::BeginDisabled();
      //if (ImGui::Selectable("Duplicate")) {
      //  //mEntityManager.CopyEntity(mRightClickedEntity);
      //  Reflection::ObjectFactory::GetInstance().CloneObject(mRightClickedEntity);
      //}
      //ImGui::EndDisabled();

      if (mEditingPrefab) { ImGui::BeginDisabled(); }
      if (ImGui::Selectable("Save as Prefab")) {
        mPrefabPopup = true;
      }
      if (mEditingPrefab) { ImGui::EndDisabled(); }

      if (ImGui::Selectable("Delete")) {
        mEntityManager.RemoveEntity(mRightClickedEntity);
      }

      ImGui::EndPopup();
    }
  }

  void SceneHierarchy::RunPrefabPopup()
  {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create Prefab", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      static std::string input{};
      static bool blankWarning{ false }, existingPrefabWarning{ false };
      static auto& prefabMan{ Prefabs::PrefabManager::GetInstance() };

      if (mFirstTimePfbPopup) {
        input = mRightClickedEntity.GetComponent<Component::Tag>().tag;
        existingPrefabWarning = prefabMan.DoesPrefabExist(input);
        mFirstTimePfbPopup = false;
      }

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
          prefabMan.CreatePrefabFromEntity(mRightClickedEntity, input);
          blankWarning = existingPrefabWarning = false;
          input.clear();
          ImGui::CloseCurrentPopup();
        }
      }

      ImGui::EndPopup();
    }
  }

} // namespace GUI
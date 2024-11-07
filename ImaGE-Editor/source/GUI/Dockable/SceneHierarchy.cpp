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
  static bool sEntityDoubleClicked{ false }, sEditNameMode{ false }, sFirstEnterEditMode{ true };
  static bool sLMouseReleased{ false };
  static float sTimeElapsed;  // for renaming entity

  SceneHierarchy::SceneHierarchy(const char* name) : GUIWindow(name),
    mEntityManager{ ECS::EntityManager::GetInstance() },
    mSceneName{},
    mRightClickedEntity{}, mRightClickMenu{ false }, mEntityOptionsMenu{ false },
    mPrefabPopup{ false }, mFirstTimePfbPopup{ true }, mEditingPrefab{ false }, mLockControls{ false }, mSceneModified{ false }
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &SceneHierarchy::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &SceneHierarchy::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_MODIFIED, &SceneHierarchy::HandleEvent, this);
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


    std::string const sceneNameSave{ mSceneName };

    if (!mEditingPrefab) {
      // Ctrl + S to save
      if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        QUEUE_EVENT(Events::SaveSceneEvent);
        if (!mSceneModified) {
          mSceneName.erase(mSceneName.size() - 2);
          mSceneModified = false;
        }
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
      if (drop) {
        ECS::Entity droppedEntity{ *reinterpret_cast<ECS::Entity*>(drop->Data) };
        if (mEntityManager.HasParent(droppedEntity)) {
          mEntityManager.RemoveParent(droppedEntity);

          // entity unparented - convert to local and recompute matrix
          Component::Transform& trans{ droppedEntity.GetComponent<Component::Transform>() };
          trans.SetLocalToWorld();
          trans.modified = true;

          QUEUE_EVENT(Events::SceneModifiedEvent);
        }
      }

      ImGui::EndDragDropTarget();
    }

    // timer before a rename is confirmed
    // this is to prevent clashing with double-clicks
    if (sEntityDoubleClicked) {
      sTimeElapsed += Performance::FrameRateController::GetInstance().GetDeltaTime();
      // set flag when click is released
      if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        sLMouseReleased = true;
      }

      if (sTimeElapsed >= sTimeBeforeRename) {
        // after target time, check whether mouse was released
        // to determine if it was a rename operation
        if (sLMouseReleased) {
          sEditNameMode = mLockControls = true;
        }
        sTimeElapsed = 0;
        sEntityDoubleClicked = false;
      }
    }
    else {
      sTimeElapsed = 0;
    }

    for (auto const& e : mEntityManager.GetAllEntities())
    {
      if (mEntityManager.HasParent(e)) { continue; }

      RecurseDownHierarchy(e);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && GUIManager::GetSelectedEntity() && !mLockControls) {
      ECS::EntityManager::GetInstance().RemoveEntity(GUIManager::GetSelectedEntity());
      GUIManager::SetSelectedEntity(ECS::Entity());
      QUEUE_EVENT(Events::SceneModifiedEvent);
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
    if (RunRightClickMenu() || RunEntityOptions()) {
      QUEUE_EVENT(Events::SceneModifiedEvent);
    }
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
    bool const isCurrentEntity{ GUIManager::GetSelectedEntity() == entity }, isEditMode{ isCurrentEntity && sEditNameMode };
    // set the flag accordingly
    ImGuiTreeNodeFlags treeFlag{ ImGuiTreeNodeFlags_SpanFullWidth };
    bool const hasChildren{ mEntityManager.HasChild(entity) };
    treeFlag |= hasChildren ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen
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
        if (sFirstEnterEditMode) {
          ImGui::SetKeyboardFocusHere();
          sFirstEnterEditMode = false;
        }
        if (ImGui::InputText("##EntityRename", &entityName, ImGuiInputTextFlags_AutoSelectAll)) {
          entity.GetComponent<Component::Tag>().tag = entityName;
          if (!mSceneModified) {
            QUEUE_EVENT(Events::SceneModifiedEvent);
          }
        }
        ImGui::PopStyleVar();

        if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
          ResetEditNameMode();
        }
      }

      ProcessInput(entity);

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

  void SceneHierarchy::ProcessInput(ECS::Entity entity) {

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
        mEntityOptionsMenu = sFirstEnterEditMode = true;
      }
    }

    // trigger event for viewport to pan the camera over to the entity
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
      QUEUE_EVENT(Events::ZoomInOnEntity, entity);
      sEntityDoubleClicked = false;
      sFirstEnterEditMode = true;
    }
    else if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      if (GUIManager::GetSelectedEntity() == entity) {
        sEntityDoubleClicked = true;
        sLMouseReleased = false;
      }
      else {
        GUIManager::SetSelectedEntity(entity);
      }
    }

    if (sEditNameMode && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) 
      || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle))) {
      ResetEditNameMode();
    }
  }

  bool SceneHierarchy::RunRightClickMenu() const
  {
    bool modified{ false };
    if (ImGui::BeginPopup("HierarchyOptions"))
    {
      if (ImGui::Selectable("Create Entity")) {
        CreateNewEntity();
        modified = true;
      }

      if (ImGui::Selectable("Create Light")) {
        ECS::Entity newEntity{ mEntityManager.CreateEntity() };
        newEntity.SetTag("Light");
        newEntity.EmplaceComponent<Component::Light>();
        modified = true;
      }

      ImGui::EndPopup();
    }

    return modified;
  }

  bool SceneHierarchy::RunEntityOptions()
  {
    bool modified{ false };
    if (ImGui::BeginPopup("EntityOptions"))
    {
      if (ImGui::Selectable("Create Entity")) {
        ECS::Entity newEntity{ CreateNewEntity() };
        mEntityManager.SetParentEntity(mRightClickedEntity, newEntity);
        modified = true;
      }

      if (mEntityManager.HasChild(mRightClickedEntity)) {
        if (ImGui::Selectable("Set Children to follow Layer")) {
          mEntityManager.SetChildLayersToFollowParent(mRightClickedEntity);
        }

        if (ImGui::Selectable("Set Children to follow Active Status")) {
          mEntityManager.SetChildActiveToFollowParent(mRightClickedEntity);
        }
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
        modified = true;
      }

      ImGui::EndPopup();
    }

    return modified;
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

  void SceneHierarchy::ResetEditNameMode() {
    ImGui::SetWindowFocus(NULL);
    sEditNameMode = mLockControls = false;
    sFirstEnterEditMode = true;
  }

} // namespace GUI
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
#include <GUI/GUIVault.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Core/Components/Components.h>
#include <EditorEvents.h>

#include <Prefabs/PrefabManager.h>
#include <Events/EventManager.h>
#include <Serialization/Serializer.h>
#include <Serialization/Deserializer.h>

#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace {
  static ECS::Entity sEntityToRename{};
  static bool sEntityDoubleClicked{ false }, sEditNameMode{ false }, sFirstEnterEditMode{ true };
  static bool sLMouseReleased{ false }, sCtrlHeld{ false }, sWasCtrlHeld{ false };
  static float sTimeElapsed;  // for renaming entity
  static bool sJumpToEntity{ false };
  // have to use these bools cause we can't get ImGui ID stack out of window scope
  static bool sSaveHierarchyState{ false }, sLoadHierarchyState{ false };

  void RemovePrefabOverrides(ECS::Entity root, IGE::Assets::GUID guid);
  ECS::Entity GetPrefabRoot(ECS::Entity root);
  void CopyWorldTransform(Component::Transform const& source, Component::Transform& dest);

  void ReassignSubmeshIndices(ECS::Entity root);
  ECS::Entity GetCanvasEntity();
}

namespace GUI
{
  SceneHierarchy::SceneHierarchy(const char* name) : GUIWindow(name),
    mCollapsedNodes{}, mEntityManager{ ECS::EntityManager::GetInstance() },
    mSceneName{},
    mRightClickedEntity{}, mEntityOptionsMenu{ false }, mEntityRightClicked{ false },
    mPrefabPopup{ false }, mFirstTimePfbPopup{ true }, mEditingPrefab{ false }, mLockControls{ false }, mSceneModified{ false }
  {
    SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &SceneHierarchy::OnSceneStateChange, this);
    SUBSCRIBE_CLASS_FUNC(Events::EditPrefabEvent, &SceneHierarchy::OnPrefabEdit, this);
    SUBSCRIBE_CLASS_FUNC(Events::SceneModifiedEvent, &SceneHierarchy::OnSceneModified, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityScreenPicked, &SceneHierarchy::OnEntityPicked, this);
    SUBSCRIBE_CLASS_FUNC(Events::CollectEditorSceneData, &SceneHierarchy::OnCollectEditorData, this);
    SUBSCRIBE_CLASS_FUNC(Events::LoadEditorSceneData, &SceneHierarchy::OnLoadEditorData, this);

    std::string const hierarchyConfigDir{ std::string(gEditorAssetsDirectory) + "Scenes" };
    if (!std::filesystem::is_directory(hierarchyConfigDir)) {
      std::filesystem::create_directory(hierarchyConfigDir);
    }
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
        QUEUE_EVENT(Events::SaveSceneEvent, GUIVault::sSerializePrettyScene);
        if (mSceneModified) {
          mSceneName.erase(mSceneName.size() - 2);
          mSceneModified = false;
        }

        sSaveHierarchyState = true;
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
          SceneModified();
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
        if (sLMouseReleased && !mEditingPrefab) {
          sEditNameMode = mLockControls = true;
          sEntityToRename = GUIVault::GetSelectedEntity();
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

      RecurseDownHierarchy(e, ImGui::GetTreeNodeToLabelSpacing());
    }

    if (sSaveHierarchyState) {
      // save the hieararchy
      mCollapsedNodes = GetTreeNodeStates();
      sSaveHierarchyState = false;
    }
    else if (sLoadHierarchyState) {
      LoadHierarchyState();
      sLoadHierarchyState = false;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && GUIVault::GetSelectedEntity() && !mLockControls) {
      ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
      auto entities{ GUIVault::GetSelectedEntities() };
      if (!entities.empty()) {
        for (ECS::Entity e : entities) {
          em.RemoveEntity(e);
        }
        GUIVault::ClearSelectedEntities();
      }
      else {
        em.RemoveEntity(GUIVault::GetSelectedEntity());
      }

      GUIVault::SetSelectedEntity(ECS::Entity());
      SceneModified();
    }

    sCtrlHeld = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
    if (sCtrlHeld) { sWasCtrlHeld = true; }

    if (mEntityOptionsMenu) {
      ImGui::OpenPopup("HierarchyOptions");
      mEntityOptionsMenu = false;
      mEntityRightClicked = true;
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
      ImGui::OpenPopup("HierarchyOptions");
    }
    else if (mPrefabPopup) {
      ImGui::OpenPopup("Create Prefab");
      mPrefabPopup = false;
      mFirstTimePfbPopup = true;
    }

    if (RunRightClickMenu(mEntityRightClicked)) {
      SceneModified();
    }
    
    RunPrefabPopup();

    ImGui::End();
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnSceneStateChange)
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
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnPrefabEdit) {
    mSceneName = CAST_TO_EVENT(Events::EditPrefabEvent)->mPrefab;
    mEditingPrefab = true;
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnSceneModified) {
    if (mSceneModified) { return; }

    mSceneName += " *";
    mSceneModified = true;
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnEntityPicked) {
    sJumpToEntity = true;
  }

  ECS::Entity SceneHierarchy::CreateNewEntity() const
  {
    auto newEntity{ mEntityManager.CreateEntity() };
    newEntity.GetComponent<Component::Tag>().tag = "Entity " + std::to_string(newEntity.GetEntityID());
    return newEntity;
  }

  void SceneHierarchy::RecurseDownHierarchy(ECS::Entity entity, float nodeToLabelSpacing)
  {
    bool const isCurrentEntity{ GUIVault::GetSelectedEntity() == entity || GUIVault::IsEntitySelected(entity)},
      isEditMode{ sEntityToRename == entity && sEditNameMode };
    // set the flag accordingly
    ImGuiTreeNodeFlags treeFlag{ ImGuiTreeNodeFlags_SpanFullWidth };
    bool const hasChildren{ mEntityManager.HasChild(entity) };
    treeFlag |= hasChildren ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen
      : ImGuiTreeNodeFlags_Leaf;

    if (isCurrentEntity) { treeFlag |= ImGuiTreeNodeFlags_Selected; }

    // create the tree nodes
    std::string const displayName{ isEditMode ? "##" : entity.GetComponent<Component::Tag>().tag };

    // highlight if its a prefab instance
    bool const isPrefabInstance{ entity.HasComponent<Component::PrefabOverrides>() };
    if (isPrefabInstance) {
      ImGui::PushStyleColor(ImGuiCol_Text, sEntityHighlightCol);
    }

    if (!entity.IsActive()) {
      ImGui::PushStyleColor(ImGuiCol_Text, isPrefabInstance ? sInactivePfbInstCol : sEntityInactiveCol);
    }
    if (ImGui::TreeNodeEx((displayName + "##" + std::to_string(entity.GetEntityID())).c_str(), treeFlag))
    {
      if (isPrefabInstance) { ImGui::PopStyleColor(); }
      if (!entity.IsActive()) { ImGui::PopStyleColor(); }

      // if renaming entity
      if (isEditMode) { RenameEntity(entity); }

      ProcessInput(entity, false, nodeToLabelSpacing);

      // if entity picked from viewport, set scroll position to it
      if (sJumpToEntity && GUIVault::GetSelectedEntity() == entity) {
        ImGui::SetScrollHereY(0.1f);
        sJumpToEntity = false;
      }

      if (hasChildren) {
        nodeToLabelSpacing += ImGui::GetTreeNodeToLabelSpacing();
        for (auto const& child : mEntityManager.GetChildEntity(entity)) {
          RecurseDownHierarchy(child, nodeToLabelSpacing);
        }
      }

      ImGui::TreePop();
    }
    else {
      if (!entity.IsActive()) { ImGui::PopStyleColor(); }
      if (isPrefabInstance) { ImGui::PopStyleColor(); }

      // if renaming entity
      if (isEditMode) { RenameEntity(entity); }

      ProcessInput(entity, true, nodeToLabelSpacing);

      // if entity picked from viewport, set scroll position to it
      if (sJumpToEntity && GUIVault::GetSelectedEntity() == entity) {
        ImGui::SetScrollHereY(0.1f);
        sJumpToEntity = false;
      }
    }
  }

  void SceneHierarchy::RenameEntity(ECS::Entity entity) {
    std::string entityName{ entity.GetComponent<Component::Tag>().tag };

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
      SceneModified();
    }
    ImGui::PopStyleVar();
    if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
      && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
      ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
      ResetEditNameMode();
    }
  }

  void SceneHierarchy::ProcessInput(ECS::Entity entity, bool collapsed, float nodeToLabelSpacing) {

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
        // check for entity dragdrop
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
          SceneModified();
        }
        // else check for asset dragdrop
        else if (ImGuiHelpers::AssetDragDropBehavior(entity)) {
          // if successful, set target to selected
          GUIVault::SetSelectedEntity(entity);
        }
        ImGui::EndDragDropTarget();
      }

      if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        mRightClickedEntity = entity;
        mEntityOptionsMenu = true;
      }
    }

    // trigger event for viewport to pan the camera over to the entity
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
      QUEUE_EVENT(Events::ZoomInOnEntity, entity);
      sEntityDoubleClicked = false;
    }
    else if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > nodeToLabelSpacing) {
      if (GUIVault::GetSelectedEntity() == entity && !sCtrlHeld && !collapsed) {
        sEntityDoubleClicked = true;
        sLMouseReleased = false;
      }
      else {
        if (sCtrlHeld) {
          ECS::Entity const curr{ GUIVault::GetSelectedEntity() };
          if (GUIVault::GetSelectedEntities().empty() && curr) {
            GUIVault::AddSelectedEntity(curr);
          }

          if (GUIVault::IsEntitySelected(entity)) {
            GUIVault::RemoveSelectedEntity(entity);
            if (GUIVault::GetSelectedEntities().empty()) {
              GUIVault::SetSelectedEntity({});
            }
            else {
              GUIVault::SetSelectedEntity(*GUIVault::GetSelectedEntities().begin());
            }
          }
          else {
            GUIVault::AddSelectedEntity(entity);
            GUIVault::SetSelectedEntity(entity);
          }
        }
        else {
          if (sWasCtrlHeld) {
            GUIVault::ClearSelectedEntities();
            sWasCtrlHeld = false;
          }

          GUIVault::SetSelectedEntity(entity);
        }
      }
    }

    if (sEditNameMode && (ImGui::IsMouseClicked(ImGuiMouseButton_Left)
      || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle))) {
      ResetEditNameMode();
    }
  }

  bool SceneHierarchy::RunRightClickMenu(bool entitySelected)
  {
    bool modified{ false };
    if (ImGui::BeginPopup("HierarchyOptions"))
    {
      // Reassign Submesh Indices
      if (GUIVault::sDevTools && entitySelected) {
        if (!mRightClickedEntity.HasComponent<Component::Mesh>()) {
          ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 0, 0, 255));
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
          if (ImGui::Selectable("Reassign Submesh Indices")) {
            ReassignSubmeshIndices(mRightClickedEntity);
          }
          ImGui::PopStyleColor(2);
        }

        ImGui::Separator();
      }

      // if entity is right-clicked
      if (entitySelected) {
        if (mEntityManager.HasChild(mRightClickedEntity)) {
          if (ImGui::Selectable("Set Children to follow Layer")) {
            mEntityManager.SetChildLayersToFollowParent(mRightClickedEntity);
            modified = true;
          }

          if (ImGui::Selectable("Set Children to follow Active Status")) {
            mEntityManager.SetChildActiveToFollowParent(mRightClickedEntity);
            modified = true;
          }

          ImGui::Separator();
        }

        if (ImGui::Selectable("Rename")) {
          sEditNameMode = mLockControls = true;
          sEntityToRename = mRightClickedEntity;
        }

        if (ImGui::Selectable("Duplicate")) {
          ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
          ECS::Entity newEntity{ Reflection::ObjectFactory::GetInstance().CloneObject(mRightClickedEntity,
            em.HasParent(mRightClickedEntity) ? em.GetParentEntity(mRightClickedEntity) : ECS::Entity()) };
          newEntity.GetComponent<Component::Tag>().tag += " (Copy)";
          GUIVault::SetSelectedEntity(newEntity);
          TriggerRename();

          modified = true;
        }

        if (!mEditingPrefab && ImGui::Selectable("Save as Prefab")) {
          mPrefabPopup = modified = true;
        }

        if (ImGui::Selectable("Delete")) {
          mEntityManager.RemoveEntity(mRightClickedEntity);
          modified = true;
        }

        ImGui::Separator();
      }

      // Prefab menu
      if (entitySelected && mRightClickedEntity.HasComponent<Component::PrefabOverrides>()) {
        if (PrefabMenu("Prefab")) {
          modified = true;
        }

        ImGui::Separator();
      }

      // Create Entity
      if (ImGui::Selectable("Create Entity")) {
        ECS::Entity newEntity{ CreateNewEntity() };

        if (entitySelected) {
          ParentRightClickedEntity(newEntity);
        }
        GUIVault::SetSelectedEntity(newEntity);
        modified = true;
        TriggerRename();
      }

      // Create Empty Parent
      if (entitySelected) {
        if (CreateEmptyParent("Create Empty Parent")) {
          modified = true;
          TriggerRename();
        }
      }

      // Create Object
      if (MeshMenu("Object", entitySelected)) {
        modified = true;
        TriggerRename();
      }

      // Create Light
      if (LightMenu("Light", entitySelected)) {
        modified = true;
        TriggerRename();
      }

      if (AudioMenu("Audio", entitySelected)) {
        modified = true;
        TriggerRename();
      }

      if (ImGui::Selectable("Camera")) {
        ECS::Entity newEntity{ mEntityManager.CreateEntityWithTag("Camera") };
        newEntity.EmplaceComponent<Component::Camera>().fov = 60.f;

        if (entitySelected) {
          ParentRightClickedEntity(newEntity);
        }

        GUIVault::SetSelectedEntity(newEntity);
        modified = true;
        TriggerRename();
      }

      // Create UI
      if (UIMenu("UI", entitySelected)) {
        modified = true;
        TriggerRename();
      }


      ImGui::EndPopup();
    }
    // reset flag for entity selected
    else if (mEntityRightClicked) {
      mEntityRightClicked = false;
    }

    return modified;
  }

  bool SceneHierarchy::MeshMenu(const char* label, bool entitySelected) {
    const char* meshName{ nullptr };
    if (ImGui::BeginMenu(label)) {
      if (ImGui::Selectable("Cube")) { meshName = "Cube"; }

      if (ImGui::Selectable("Sphere")) { meshName = "Sphere"; }

      if (ImGui::Selectable("Plane")) { meshName = "Plane"; }

      if (ImGui::Selectable("Capsule")) { meshName = "Capsule"; }

      if (ImGui::Selectable("Quad")) { meshName = "Quad"; }

      if (meshName) {
        ECS::Entity newEntity{ mEntityManager.CreateEntityWithTag(meshName) };
        newEntity.EmplaceComponent<Component::Mesh>(meshName);
        GUIVault::SetSelectedEntity(newEntity);

        if (entitySelected) {
          ParentRightClickedEntity(newEntity);
        }
      }

      ImGui::EndMenu();
    }

    return meshName;
  }

  bool SceneHierarchy::LightMenu(const char* label, bool entitySelected) {
    bool modified{ false };
    if (ImGui::BeginMenu(label)) {
      ECS::Entity newEntity{};

      if (ImGui::Selectable("Directional")) {
        newEntity = mEntityManager.CreateEntityWithTag("Directional Light");
        newEntity.GetComponent<Component::Transform>().ApplyWorldRotation(-90.f, glm::vec3(1.f, 0.f, 0.f));  // face down by default
        newEntity.EmplaceComponent<Component::Light>(Component::DIRECTIONAL);
        modified = true;
      }

      if (ImGui::Selectable("Spot Light")) {
        newEntity = mEntityManager.CreateEntityWithTag("Spot Light");
        newEntity.GetComponent<Component::Transform>().ApplyWorldRotation(-90.f, glm::vec3(1.f, 0.f, 0.f));  // face down by default
        newEntity.EmplaceComponent<Component::Light>(Component::SPOTLIGHT);
        modified = true;
      }

      if (ImGui::Selectable("Point Light")) {
        newEntity = mEntityManager.CreateEntityWithTag("Point Light");
        newEntity.EmplaceComponent<Component::Light>(Component::POINT);
        modified = true;
      }

      if (modified) {
        GUIVault::SetSelectedEntity(newEntity);

        if (entitySelected) {
          ParentRightClickedEntity(newEntity);
        }
      }

      ImGui::EndMenu();
    }

    return modified;
  }

  bool SceneHierarchy::AudioMenu(const char* label, bool entitySelected) {
    ECS::Entity newEntity{};
    if (ImGui::BeginMenu(label)) {

      if (ImGui::Selectable("Audio Source")) {
        newEntity = mEntityManager.CreateEntityWithTag("Audio Source");
        newEntity.EmplaceComponent<Component::AudioSource>();
      }

      if (ImGui::Selectable("Audio Listener")) {
        newEntity = mEntityManager.CreateEntityWithTag("Audio Listener");
        newEntity.EmplaceComponent<Component::AudioListener>();
      }

      if (newEntity) {
        GUIVault::SetSelectedEntity(newEntity);

        if (entitySelected) {
          ParentRightClickedEntity(newEntity);
        }
      }

      ImGui::EndMenu();
    }

    return newEntity;
  }

  bool SceneHierarchy::UIMenu(const char* label, bool entitySelected) {
    ECS::Entity newEntity{};
    if (ImGui::BeginMenu(label)) {
      bool canvasCreated{ false };
      if (ImGui::Selectable("Image")) {
        newEntity = mEntityManager.CreateEntityWithTag("Image");
        newEntity.EmplaceComponent<Component::Image>();
      }

      if (ImGui::Selectable("Text")) {
        newEntity = mEntityManager.CreateEntityWithTag("Text");
        newEntity.EmplaceComponent<Component::Text>();
      }

      ImGui::Separator();

      if (ImGui::Selectable("Canvas")) {
        newEntity = mEntityManager.CreateEntityWithTag("Canvas");
        newEntity.EmplaceComponent<Component::Canvas>();
        canvasCreated = true;
      }

      if (newEntity) {
        GUIVault::SetSelectedEntity(newEntity);

        if (entitySelected) {
          // check if under canvas
          bool const valid{ mRightClickedEntity.HasComponent<Component::Canvas>()
              || ImGuiHelpers::IsUnderCanvasEntity(mRightClickedEntity) };

          // if canvas created or under canvas, simply nest under the right-clicked entity
          if (canvasCreated || valid) {
            ParentRightClickedEntity(newEntity);
          }
          // else we create a new canvas to hold the new entity,
          // then set the canvas to be the child of the right-clicked entity
          else {
            ECS::Entity canvas{ mEntityManager.CreateEntityWithTag("Canvas") };
            canvas.EmplaceComponent<Component::Canvas>();

            mEntityManager.SetParentEntity(canvas, newEntity);
            mEntityManager.SetParentEntity(mRightClickedEntity, canvas);

            // set world transform of both canvas and new entity equal to the parent
            Component::Transform& canvasTrans{ canvas.GetComponent<Component::Transform>() };
            CopyWorldTransform(mRightClickedEntity.GetComponent<Component::Transform>(), canvasTrans);
            CopyWorldTransform(canvasTrans, newEntity.GetComponent<Component::Transform>());
          }
        }
        // entity created at root level - look for the canvas
          // and set the new entity as its child
        else if (!canvasCreated) {
          ECS::Entity canvas{ GetCanvasEntity() };
          if (!canvas) {
            canvas = mEntityManager.CreateEntityWithTag("Canvas");
            canvas.EmplaceComponent<Component::Canvas>();
          }

          mEntityManager.SetParentEntity(canvas, newEntity);
          CopyWorldTransform(canvas.GetComponent<Component::Transform>(), newEntity.GetComponent<Component::Transform>());
        }
      }

      ImGui::EndMenu();
    }

    return newEntity;
  }

  bool SceneHierarchy::CreateEmptyParent(const char* label) {
    if (!ImGui::Selectable(label)) { return false; }

    ECS::Entity newEntity{ CreateNewEntity() };

    // if original entity has a parent, set this new entity as a child
    if (mEntityManager.HasParent(mRightClickedEntity)) {
      ECS::Entity const originalParent{ mEntityManager.GetParentEntity(mRightClickedEntity) };
      mEntityManager.RemoveParent(mRightClickedEntity);
      CopyWorldTransform(originalParent.GetComponent<Component::Transform>(), newEntity.GetComponent<Component::Transform>());
      mEntityManager.SetParentEntity(originalParent, newEntity);
    }
    else {
      Component::Transform& newTrans{ newEntity.GetComponent<Component::Transform>() },
        & rightClickedTrans{ mRightClickedEntity.GetComponent<Component::Transform>() };
      newTrans = rightClickedTrans;
      rightClickedTrans.ResetLocal();
      newTrans.SetLocalToWorld();
    }

    mEntityManager.SetParentEntity(newEntity, mRightClickedEntity);
    /*mRightClickedEntity.GetComponent<Component::Transform>().ResetLocal();
    TransformHelpers::UpdateTransformToNewParent(mRightClickedEntity);*/
    GUIVault::SetSelectedEntity(newEntity);

    return true;
  }

  bool SceneHierarchy::PrefabMenu(const char* label) {
    Component::PrefabOverrides& overrides{ mRightClickedEntity.GetComponent<Component::PrefabOverrides>() };
    bool modified{ false };

    if (ImGui::BeginMenu(label)) {
      if (ImGui::MenuItem("Reset All Overrides")) {
        try {
          Reflection::ObjectFactory& of{ IGE_OBJFACTORY };
          Prefabs::Prefab const& pfb{ IGE_ASSETMGR.GetAsset<IGE::Assets::PrefabAsset>(overrides.guid)->mPrefabData };
          std::vector<rttr::variant> const comps{ of.GetEntityComponents(mRightClickedEntity) };

          for (rttr::variant const& comp : comps) {
            rttr::variant const& compToRestore{ pfb.GetSubObject(overrides.subDataId).GetComponent(comp.get_type()) };

            if (compToRestore.is_valid()) {
              of.AddComponentToEntity(mRightClickedEntity, compToRestore);
            }
            else {
              of.RemoveComponentFromEntity(mRightClickedEntity, comp.get_type());
            }
          }
        }
        catch (Debug::ExceptionBase&) {
          IGE_DBGLOGGER.LogError("Unable to fetch prefab " + std::to_string(overrides.guid));
        }

        overrides.Reset();
        modified = true;
      }

      if (overrides.IsRoot()) {
        // only allow detaching from the root entity
        if (ImGui::MenuItem("Detach Instance")) {
          RemovePrefabOverrides(mRightClickedEntity, overrides.guid);
          modified = true;
        }
      }
      else {
        // get root entity of prefab
        if (ImGui::MenuItem("Select Root")) {
          GUIVault::SetSelectedEntity(GetPrefabRoot(mRightClickedEntity));
        }
      }

      ImGui::EndMenu();
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
    sEditNameMode = mLockControls = false;
    sFirstEnterEditMode = true;
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnCollectEditorData) {
    auto sceneDataEvent{ CAST_TO_EVENT(Events::CollectEditorSceneData) };
    sceneDataEvent->mSceneName = mSceneName;
    sceneDataEvent->mSceneConfig.collapsedNodes = std::move(mCollapsedNodes);
    mCollapsedNodes = {};
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnLoadEditorData) {
    mCollapsedNodes = CAST_TO_EVENT(Events::LoadEditorSceneData)->mSceneConfig.collapsedNodes;
    sLoadHierarchyState = true;
  }

  void GetTreeNodeStatesR(std::set<HierarchyEntry>& collapsed, ECS::Entity entity, ImGuiID parentHash) {
    ImGuiID const newHash{ ImGuiHelpers::GetTreeNodeId(entity.GetTag() + "##" + std::to_string(entity.GetEntityID()), parentHash)};
    
    bool const state{ ImGui::TreeNodeBehaviorIsOpen(newHash, ImGuiTreeNodeFlags_DefaultOpen) };
    collapsed.emplace(entity.GetRawEnttEntityID(), newHash, state);
    //std::cout << entity.GetTag() << "##" << entity.GetEntityID() << " is closed - " << std::hex << newHash << "\n";

    ECS::EntityManager& em{ IGE_ENTITYMGR };
    for (ECS::Entity child : IGE_ENTITYMGR.GetChildEntity(entity)) {
      if (!em.HasChild(child)) { continue; }

      GetTreeNodeStatesR(collapsed, child, newHash);
    }
  }

  std::set<HierarchyEntry> SceneHierarchy::GetTreeNodeStates() const {
    std::set<HierarchyEntry> ret{};
    ECS::EntityManager& em{ IGE_ENTITYMGR };

    for (ECS::Entity e : em.GetAllEntities()) {
      if (em.HasParent(e) || !em.HasChild(e)) { continue; }

      GetTreeNodeStatesR(ret, e, ImHashStr(mWindowName.c_str()));
    }

    return ret;
  }

  void SceneHierarchy::TriggerRename() {
    sEntityToRename = GUIVault::GetSelectedEntity();
    sEditNameMode = mLockControls = true;
  }

  void SceneHierarchy::ParentRightClickedEntity(ECS::Entity child) {
    mEntityManager.SetParentEntity(mRightClickedEntity, child);
    CopyWorldTransform(mRightClickedEntity.GetComponent<Component::Transform>(), child.GetComponent<Component::Transform>());
  }

  void SceneHierarchy::LoadHierarchyState() {
    ImGuiStorage* stateStore{ ImGui::GetStateStorage() };
    for (GUI::HierarchyEntry const& entry : mCollapsedNodes) {
      stateStore->SetInt(entry.stackId, entry.isOpen ? 1 : 0);
    }
  }

  void SceneHierarchy::SceneModified() {
    if (mSceneModified) { return; }

    QUEUE_EVENT(Events::SceneModifiedEvent);
  }

} // namespace GUI

namespace {
  void RemovePrefabOverrides(ECS::Entity root, IGE::Assets::GUID guid) {
    if (!root.HasComponent<Component::PrefabOverrides>()
      || root.GetComponent<Component::PrefabOverrides>().guid != guid) {
      return;
    }

    // remove overrides component
    root.RemoveComponent<Component::PrefabOverrides>();

    // do the same for each child
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
    if (em.HasChild(root)) {
      for (ECS::Entity child : em.GetChildEntity(root)) {
        RemovePrefabOverrides(child, guid);
      }
    }
  }

  ECS::Entity GetPrefabRoot(ECS::Entity root) {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };

    IGE::Assets::GUID const guid{ root.GetComponent<Component::PrefabOverrides>().guid };
    while (em.HasParent(root)) {
      ECS::Entity const parent{ em.GetParentEntity(root) };
      Component::PrefabOverrides const& overrides{ parent.GetComponent<Component::PrefabOverrides>() };
      if (overrides.guid == guid && overrides.IsRoot()) { return parent; }

      root = parent;
    }

    IGE_DBGLOGGER.LogError("[SceneHirarchy] Unable to get prefab root!");
    return {};
  }

  void CopyWorldTransform(Component::Transform const& source, Component::Transform& dest) {
    dest.worldPos = source.worldPos;
    dest.worldScale = source.worldScale;
    dest.worldRot = source.worldRot;
    dest.worldMtx = source.worldMtx;
  }

  void ReassignSubmeshIndices(ECS::Entity root) {
    ECS::EntityManager& em{ IGE_ENTITYMGR };
    if (!em.HasChild(root)) { return; }

    unsigned idx{};
    for (ECS::Entity child : em.GetChildEntity(root)) {
      if (!child.HasComponent<Component::Mesh>()) { continue; }

      child.GetComponent<Component::Mesh>().submeshIdx = idx;
      ++idx;
    }
  }

  ECS::Entity GetCanvasEntity() {
    auto const canvases{ IGE_ENTITYMGR.GetAllEntitiesWithComponents<Component::Canvas>() };
    return canvases.empty() ? ECS::Entity() : canvases.front();
  }
}
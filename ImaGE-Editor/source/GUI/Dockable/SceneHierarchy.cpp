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
#include <Core/Components/Tag.h>
#include <GUI/GUIVault.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Events/EventManager.h>
#include <Prefabs/PrefabManager.h>
#include <GUI/Dockable/Inspector.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Serialization/Deserializer.h>
#include <Serialization/Serializer.h>

#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace {
  static bool sEntityDoubleClicked{ false }, sEditNameMode{ false }, sFirstEnterEditMode{ true };
  static bool sLMouseReleased{ false }, sCtrlHeld{ false }, sWasCtrlHeld{ false };
  static float sTimeElapsed;  // for renaming entity
  static bool sJumpToEntity{ false }, sSaveHierarchyState{ false };
  static std::string sLoadHierarchyPath{};

  void RemovePrefabOverrides(ECS::Entity root, IGE::Assets::GUID guid);
  ECS::Entity GetPrefabRoot(ECS::Entity root);
  void CopyWorldTransform(Component::Transform const& source, Component::Transform& dest);

  void ReassignSubmeshIndices(ECS::Entity root);
  void LoadHierarchyState();
}

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
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ENTITY_PICKED, &SceneHierarchy::OnEntityPicked, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOAD_SCENE, &SceneHierarchy::OnSceneLoad, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SAVE_SCENE, &SceneHierarchy::OnSceneSave, this);

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

    if (sSaveHierarchyState) {
      HierarchyConfig hierarchyCfg{ GetTreeNodeStates() };
      Serialization::Serializer::SerializeAny(hierarchyCfg, gEditorAssetsDirectory + std::string("Scenes\\") + mSceneName);
      sSaveHierarchyState = false;
    }
    else if (!sLoadHierarchyPath.empty()) {
      LoadHierarchyState();
      sLoadHierarchyPath.clear();
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

    if (ImGui::IsKeyPressed(ImGuiKey_B)) {
      ImGui::GetStateStorage()->SetInt(1261679633, 0);
      std::cout << std::hex << 1261679633 << " set to open\n";
    }

    sCtrlHeld = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
    if (sCtrlHeld) { sWasCtrlHeld = true; }

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
      SceneModified();
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

  EVENT_CALLBACK_DEF(SceneHierarchy, OnEntityPicked) {
    sJumpToEntity = true;
  }

  ECS::Entity SceneHierarchy::CreateNewEntity() const
  {
    auto newEntity{ mEntityManager.CreateEntity() };
    newEntity.GetComponent<Component::Tag>().tag = "Entity " + std::to_string(newEntity.GetEntityID());
    return newEntity;
  }

  void SceneHierarchy::RecurseDownHierarchy(ECS::Entity entity)
  {
    bool const isCurrentEntity{ GUIVault::GetSelectedEntity() == entity || GUIVault::IsEntitySelected(entity) },
      isEditMode{ isCurrentEntity && sEditNameMode };
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

      ProcessInput(entity, false);

      // if entity picked from viewport, set scroll position to it
      if (sJumpToEntity && GUIVault::GetSelectedEntity() == entity) {
        ImGui::SetScrollHereY(0.1f);
        sJumpToEntity = false;
      }

      if (hasChildren) {
        for (auto const& child : mEntityManager.GetChildEntity(entity)) {
          RecurseDownHierarchy(child);
        }
      }

      ImGui::TreePop();
    }
    else {
      if (!entity.IsActive()) { ImGui::PopStyleColor(); }
      if (isPrefabInstance) { ImGui::PopStyleColor(); }

      // if renaming entity
      if (isEditMode) { RenameEntity(entity); }

      ProcessInput(entity, true);

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

  void SceneHierarchy::ProcessInput(ECS::Entity entity, bool collapsed) {

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

      if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && !collapsed) {
        mRightClickedEntity = entity;
        mEntityOptionsMenu = true;
      }
    }

    // trigger event for viewport to pan the camera over to the entity
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
      QUEUE_EVENT(Events::ZoomInOnEntity, entity);
      sEntityDoubleClicked = false;
    }
    else if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
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

  bool SceneHierarchy::RunRightClickMenu()
  {
    bool modified{ false };
    if (ImGui::BeginPopup("HierarchyOptions"))
    {
      if (ImGui::Selectable("Create Entity")) {
        ECS::Entity const newEntity{ CreateNewEntity() };
        GUIVault::SetSelectedEntity(newEntity);
        modified = sEditNameMode = mLockControls = true;
      }

      if (ImGui::Selectable("Create Light")) {
        ECS::Entity newEntity{ mEntityManager.CreateEntity() };
        newEntity.SetTag("Light");
        newEntity.EmplaceComponent<Component::Light>();
        newEntity.GetComponent<Component::Transform>().ApplyWorldRotation(-90.f, glm::vec3(1.f, 0.f, 0.f));  // face down by default
        GUIVault::SetSelectedEntity(newEntity);
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
      if (GUIVault::sDevTools) {
        if (!mRightClickedEntity.HasComponent<Component::Mesh>()) {
          ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 0, 0, 255));
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
          if (ImGui::Selectable("Reassign Submesh Indices")) {
            ReassignSubmeshIndices(mRightClickedEntity);
          }
          ImGui::PopStyleColor(2);
        }
      }

      if (ImGui::Selectable("Create Entity")) {
        ECS::Entity newEntity{ CreateNewEntity() };
        mEntityManager.SetParentEntity(mRightClickedEntity, newEntity);
        CopyWorldTransform(mRightClickedEntity.GetComponent<Component::Transform>(), newEntity.GetComponent<Component::Transform>());
        GUIVault::SetSelectedEntity(newEntity);
        modified = sEditNameMode = mLockControls = true;
      }

      if (ImGui::Selectable("Create Empty Parent")) {
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
        modified = sEditNameMode = mLockControls = true;
      }

      if (mEntityManager.HasChild(mRightClickedEntity)) {
        if (ImGui::Selectable("Set Children to follow Layer")) {
          mEntityManager.SetChildLayersToFollowParent(mRightClickedEntity);
        }

        if (ImGui::Selectable("Set Children to follow Active Status")) {
          mEntityManager.SetChildActiveToFollowParent(mRightClickedEntity);
        }
      }

      if (ImGui::Selectable("Duplicate")) {
        ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
        ECS::Entity newEntity{ Reflection::ObjectFactory::GetInstance().CloneObject(mRightClickedEntity,
          em.HasParent(mRightClickedEntity) ? em.GetParentEntity(mRightClickedEntity) : ECS::Entity()) };
        newEntity.GetComponent<Component::Tag>().tag += " (Copy)";
        GUIVault::SetSelectedEntity(newEntity);
      }

      if (!mEditingPrefab && ImGui::Selectable("Save as Prefab")) {
        mPrefabPopup = true;
      }

      if (mRightClickedEntity.HasComponent<Component::PrefabOverrides>())
      {
        Component::PrefabOverrides& overrides{ mRightClickedEntity.GetComponent<Component::PrefabOverrides>() };
        if (ImGui::BeginMenu("Prefab")) {
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
      }

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
    sEditNameMode = mLockControls = false;
    sFirstEnterEditMode = true;
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnSceneLoad) {
    auto sceneLoadEvent{ CAST_TO_EVENT(Events::LoadSceneEvent) };
    sLoadHierarchyPath = gEditorAssetsDirectory + std::string("Scenes\\") + sceneLoadEvent->mSceneName;
  }

  EVENT_CALLBACK_DEF(SceneHierarchy, OnSceneSave) {
    sSaveHierarchyState = true;
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

  HierarchyConfig SceneHierarchy::GetTreeNodeStates() const {
    HierarchyConfig ret{};
    ECS::EntityManager& em{ IGE_ENTITYMGR };

    for (ECS::Entity e : em.GetAllEntities()) {
      if (em.HasParent(e) || !em.HasChild(e)) { continue; }

      GetTreeNodeStatesR(ret.collapsedNodes, e, ImHashStr(mWindowName.c_str()));
    }

    return ret;
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

  void LoadHierarchyState() {
    if (!std::filesystem::exists(sLoadHierarchyPath)) { return; }

    GUI::HierarchyConfig hierarchyCfg{};
    Serialization::Deserializer::DeserializeAny(hierarchyCfg, sLoadHierarchyPath);
    ImGuiStorage* stateStore{ ImGui::GetStateStorage() };
    for (GUI::HierarchyEntry const& entry : hierarchyCfg.collapsedNodes) {
      stateStore->SetInt(entry.stackId, entry.isOpen ? 1 : 0);
    }
  }
}
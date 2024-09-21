#include <pch.h>
#ifndef IMGUI_DISABLE
#include "SceneHierarchy.h"
#include <imgui/imgui.h>
#include <Core/Component/Tag.h>
#include <GUI/GUIManager.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Events/EventManager.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Prefabs/PrefabManager.h>

namespace GUI
{

  SceneHierarchy::SceneHierarchy(std::string const& name) : GUIWindow(name),
    mEntityManager{ ECS::EntityManager::GetInstance() },
    mSceneName{}, 
    mRightClickedEntity{}, mRightClickMenu{ false }, mEntityOptionsMenu{ false },
    mPrefabPopup{ false }, mFirstTimePfbPopup{ true }, mEditingPrefab{ false }
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &SceneHierarchy::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &SceneHierarchy::HandleEvent, this);
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

    if (mEditingPrefab) {
      ImGui::Text(("Editing Prefab: " + mSceneName).c_str());
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(253, 208, 23, 255));
      ImGui::Text("Press ESC to return to scene");
      ImGui::PopStyleColor();
    }
    else {
      ImGui::Text(mSceneName.c_str());
    }
    ImGui::Separator();

    if (ImGuiHelpers::BeginDrapDropTargetWindow(sDragDropPayload))
    {
      ImGuiPayload const* drop{ ImGui::AcceptDragDropPayload(sDragDropPayload) };
      ECS::Entity const droppedEntity{ *reinterpret_cast<ECS::Entity*>(drop->Data) };
      mEntityManager.RemoveParent(droppedEntity);

      ImGui::EndDragDropTarget();
    }

    for (auto const& e : mEntityManager.GetAllEntities())
    {
      if (!mEntityManager.HasParent(e)) {
        RecurseDownHierarchy(e);
      }
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
        mSceneName = sceneStateEvent->mSceneName;
        mEditingPrefab = false;
        break;
      case Events::SceneStateChange::STOPPED:
        mEditingPrefab = false;
        mSceneName.clear();
        break;
      default: break;
      }
    }
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
    static bool editNameMode{ false };
    // set the flag accordingly
    ImGuiTreeNodeFlags treeFlag{ ImGuiTreeNodeFlags_SpanFullWidth };
    bool const hasChildren{ mEntityManager.HasChild(entity) };
    treeFlag |= hasChildren  ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen
      : ImGuiTreeNodeFlags_Leaf;

    bool const isCurrentEntity{ GUIManager::GetSelectedEntity() == entity }, isEditMode{ isCurrentEntity && editNameMode };
    if (isCurrentEntity) { treeFlag |= ImGuiTreeNodeFlags_Selected; }

    // create the tree nodes
    std::string entityName{ entity.GetComponent<Component::Tag>().tag };
    std::string const displayName{ isEditMode ? "##" : "" + entityName };
    if (ImGui::TreeNodeEx((displayName + "##" + std::to_string(entity.GetEntityID())).c_str(), treeFlag))
    {
      if (isEditMode) {
        ImGui::SetItemAllowOverlap();
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX()  - 9.f);
        ImGui::SetKeyboardFocusHere();
        if (ImGui::InputText("##testtt", &entityName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
          entity.GetComponent<Component::Tag>().tag = entityName;
          editNameMode = false;
        }
        ImGui::PopStyleVar();
      }

      if (ImGui::BeginDragDropSource())
      {
        ECS::EntityManager::EntityID id{ entity.GetRawEnttEntityID() };
        ImGui::SetDragDropPayload(sDragDropPayload, &id, sizeof(ECS::EntityManager::EntityID), ImGuiCond_Once);
        // Anything between begin and end will be parented to the dragged object
        ImGui::Text(entityName.c_str());
        ImGui::EndDragDropSource();
      }
      if (ImGui::BeginDragDropTarget())
      {
        ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(sDragDropPayload);
        if (drop)
        {
          ECS::Entity const droppedEntity{ *reinterpret_cast<ECS::EntityManager::EntityID*>(drop->Data) };
          if (mEntityManager.HasParent(droppedEntity)) {
            mEntityManager.RemoveParent(droppedEntity);
          }
          mEntityManager.SetParentEntity(entity, droppedEntity);
        }
        ImGui::EndDragDropTarget();
      }

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        if (isCurrentEntity) {
          editNameMode = true;
        }
        else {
          GUIManager::SetSelectedEntity(entity);
          editNameMode = false;
        }
      }

      if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        mRightClickedEntity = entity;
        mEntityOptionsMenu = true;
      }

      if (hasChildren)
      {
        for (auto const& child : mEntityManager.GetChildEntity(entity))
        {
          RecurseDownHierarchy(child);
        }
      }

      ImGui::TreePop();
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
        ECS::Entity const newEntity{ CreateNewEntity() };
        mEntityManager.SetParentEntity(mRightClickedEntity, newEntity);
      }

      if (ImGui::Selectable("Duplicate")) {
        mEntityManager.CopyEntity(mRightClickedEntity);
      }

      if (ImGui::Selectable("Save as Prefab")) {
        mPrefabPopup = true;
      }

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

#endif  // IMGUI_DISABLE

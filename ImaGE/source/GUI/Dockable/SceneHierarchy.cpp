#include <pch.h>
#ifndef IMGUI_DISABLE
#include "SceneHierarchy.h"
#include <imgui/imgui.h>
#include <Core/Component/Tag.h>
#include <GUI/GUIManager.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Events/EventManager.h>

namespace GUI
{

  SceneHierarchy::SceneHierarchy(std::string const& name) 
    : mEntityManager{ ECS::EntityManager::GetInstance() },
      mRightClickedEntity{}, mRightClickMenu{ false }, mEntityOptionsMenu{ false },
      mPrefabPopup{ false }, mEditingPrefab{ false }, GUIWindow(name)
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
#ifdef HEIRARCHY_DEBUG
      std::cout << "Unparented Entity " << droppedEntity << "\n";
#endif

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
    RunRightClickMenu();
    RunEntityOptions();

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
    // set the flag accordingly
    ImGuiTreeNodeFlags treeFlag{ ImGuiTreeNodeFlags_SpanFullWidth };
    bool const hasChildren{ mEntityManager.HasChild(entity) };
    treeFlag |= hasChildren  ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen
      : ImGuiTreeNodeFlags_Leaf;

    if (GUIManager::GetSelectedEntity() == entity) { treeFlag |= ImGuiTreeNodeFlags_Selected; }

    // create the tree nodes
    std::string const& entityName{ entity.GetComponent<Component::Tag>().tag };
    if (ImGui::TreeNodeEx((entityName + "##" + std::to_string(entity.GetEntityID())).c_str(), treeFlag))
    {
      if (ImGui::BeginDragDropSource())
      {
        ECS::EntityManager::EntityID id{ entity.GetRawEnttEntityID() };
        ImGui::SetDragDropPayload(sDragDropPayload, &id, sizeof(ECS::EntityManager::EntityID));
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
        GUIManager::SetSelectedEntity(entity);
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

  void SceneHierarchy::RunRightClickMenu()
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

      ImGui::BeginDisabled();
      if (ImGui::Selectable("Save as Prefab")) {
        mPrefabPopup = true;
      }
      ImGui::EndDisabled();

      if (ImGui::Selectable("Delete")) {
        mEntityManager.RemoveEntity(mRightClickedEntity);
      }

      ImGui::EndPopup();
    }
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

#include <pch.h>
#include "SceneHierarchy.h"
#include <imgui/imgui.h>
#include <Scenes/SceneManager.h>
#include <Core/Component/Tag.h>
#include <GUI/GUIManager.h>
#include <GUI/ImGuiHelpers.h>

namespace GUI
{

  SceneHierarchy::SceneHierarchy(std::string const& name) 
    : mEntityManager{ ECS::EntityManager::GetInstance() },
      mSceneManager{ Scenes::SceneManager::GetInstance() }, 
      mRightClickedEntity{}, mRightClickMenu{ false }, mEntityOptionsMenu{ false },
      mPrefabPopup{ false }, GUIWindow(name) {}

  void SceneHierarchy::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    std::string const& sceneName{ mSceneManager.GetSceneName() };
    if (sceneName.empty())
    {
      ImGui::Text("No Scene Selected");
      ImGui::End();
      return;
    }

    ImGui::Text(sceneName.c_str());
    ImGui::Separator();

    if (ImGuiHelpers::BeginDrapDropTargetWindow(sDragDropPayload))
    {
      ImGuiPayload const* drop{ ImGui::AcceptDragDropPayload(sDragDropPayload) };
      ECS::Entity const droppedEntity{ *reinterpret_cast<ECS::Entity*>(drop->Data) };
      mEntityManager.RemoveParentEntity(droppedEntity);
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
            mEntityManager.RemoveParentEntity(droppedEntity);
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
      if (ImGui::Selectable("Create Entity"))
      {
        ECS::Entity const newEntity{ CreateNewEntity() };
        mEntityManager.SetParentEntity(mRightClickedEntity, newEntity);
      }

      ImGui::BeginDisabled();
      if (ImGui::Selectable("Save as Prefab"))
      {
        mPrefabPopup = true;
      }
      ImGui::EndDisabled();

      if (ImGui::Selectable("Delete"))
      {
        // @TODO: CHANGE WHEN ECS DELETES PROPERLY
        if (mEntityManager.HasParent(mRightClickedEntity)) {
          mEntityManager.RemoveParentEntity(mRightClickedEntity);
        }
        mEntityManager.DeleteEntity(mRightClickedEntity);
      }

      ImGui::EndPopup();
    }
  }

} // namespace GUI

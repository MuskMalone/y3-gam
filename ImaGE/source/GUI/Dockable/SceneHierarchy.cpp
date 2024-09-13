#include <pch.h>
#include "SceneHierarchy.h"
#include <imgui/imgui.h>
#include <Scenes/SceneManager.h>
#include <Core/Entity.h>

#ifdef _DEBUG
#define HEIRARCHY_DEBUG
#endif

namespace GUI
{

  SceneHierarchy::SceneHierarchy(std::string const& name) : GUIWindow(name)
  {
    
  }

  void SceneHierarchy::Run()
  {
    Scenes::SceneManager& sceneManager{ Scenes::SceneManager::GetInstance() };
    
    ImGui::Begin(mWindowName.c_str());

    ImGuiTreeNodeFlags const treeFlags{ ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow
      | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen };
    std::string const& sceneName{ sceneManager.GetSceneName() };
    if (ImGui::TreeNodeEx(sceneName.empty() ? "No scene selected" : sceneName.c_str(), treeFlags))
    {
      if (ImGui::BeginDragDropTarget())
      {
        ImGuiPayload const* drop{ ImGui::AcceptDragDropPayload(sDragDropPayload) };
        if (drop) {
          ECS::Entity const droppedEntity{ *reinterpret_cast<ECS::Entity*>(drop->Data) };
#ifdef HEIRARCHY_DEBUG
          std::cout << "Unparented Entity " << droppedEntity;
#endif
        }

        ImGui::EndDragDropTarget();
      }

      /*for (Entity const& e : mEntities)
      {
        RecurseDownHeirarchy(e);
      }*/

      ImGui::TreePop();
    }

    ImGui::End();
  }

  void SceneHierarchy::RecurseDownHeirarchy(ECS::Entity parent)
  {
    if (ImGui::TreeNodeEx(("Entity " + std::to_string(parent)).c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow
      | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {

      ImGui::TreePop();
    }
  }

} // namespace GUI

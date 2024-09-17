#pragma once
#ifndef IMGUI_DISABLE
#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "Core/Entity.h"
#include "Core/EntityManager.h"
#include <Reflection/ObjectFactory.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

    static bool const GetIsComponentEdited();
    static void SetIsComponentEdited(bool isComponentEdited);

  private:
    void ColliderComponentWindow(ECS::Entity entity);
    void LayerComponentWindow(ECS::Entity entity);
    void MaterialComponentWindow(ECS::Entity entity);
    void MeshComponentWindow(ECS::Entity entity);
    void RigidBodyComponentWindow(ECS::Entity entity);
    void ScriptComponentWindow(ECS::Entity entity);
    void TagComponentWindow(ECS::Entity entity);
    void TextComponentWindow(ECS::Entity entity);
    void TransformComponentWindow(ECS::Entity entity);
    
    template<typename Component>
    bool WindowBegin(std::string windowName);

    void WindowEnd(bool isOpen);
    void DrawAddButton();

  private:
    std::map<std::string, bool> mComponentOpenStatusMap;
    Reflection::ObjectFactory& mObjFactory;
    ECS::Entity mPreviousEntity;
    bool mEntityChanged;

    static bool sIsComponentEdited;
  };

  template<typename Component>
  bool Inspector::WindowBegin(std::string windowName) {
    ImGui::Separator();

    if (mEntityChanged) {
      bool& openMapStatus = mComponentOpenStatusMap[windowName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    bool isOpen{ ImGui::TreeNode(windowName.c_str()) };

    if (isOpen) {
      // Must close component window if a component was removed
      if (!DrawOptionsListButton<Component>(windowName)) {
        WindowEnd(true);
        return false;
      }
    }

    mComponentOpenStatusMap[windowName] = isOpen;
    return isOpen;
  }

} // namespace GUI

#endif  // IMGUI_DISABLE
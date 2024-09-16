#pragma once
#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "Core/Entity.h"
#include "Core/EntityManager.h"

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

    static bool const GetIsComponentEdited();
    static void SetIsComponentEdited(bool isComponentEdited);

  private:
    
    void LayerComponentWindow(ECS::Entity entity);
    void ScriptComponentWindow(ECS::Entity entity);
    void TagComponentWindow(ECS::Entity entity);
    void TextComponentWindow(ECS::Entity entity);
    void TransformComponentWindow(ECS::Entity entity);

    template<typename Component>
    bool WindowBegin(std::string windowName);

    void WindowEnd(bool isOpen);
    void DrawAddButton();

  private:
    static std::map<std::string, bool> sComponentOpenStatusMap;
    static ECS::Entity sPreviousEntity;
    static bool sEntityChanged;
    static bool sIsComponentEdited;
  };

  template<typename Component>
  bool Inspector::WindowBegin(std::string windowName) {
    ImGui::Separator();

    if (sEntityChanged) {
      bool& openMapStatus = sComponentOpenStatusMap[windowName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    bool isOpen{ ImGui::TreeNode(windowName.c_str()) };

    if (isOpen)
      DrawOptionsListButton<Component>(windowName);

    sComponentOpenStatusMap[windowName] = isOpen;
    return isOpen;
  }

} // namespace GUI
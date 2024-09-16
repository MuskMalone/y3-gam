#include <pch.h>
#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Inspector.h"

namespace GUI {
  // Static Initialization
  std::map<std::string, bool> Inspector::sComponentOpenStatusMap{};
  ECS::Entity Inspector::sPreviousEntity{};
  bool Inspector::sEntityChanged{};
  bool Inspector::sIsComponentEdited{ false };

  Inspector::Inspector(std::string const& name) : GUIWindow(name) {
    for (std::string const& component : Component::ComponentNameList) {
      sComponentOpenStatusMap[component] = false;
    }
  }

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());
    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    if (currentEntity) {
      if (currentEntity != sPreviousEntity) {
        sPreviousEntity = currentEntity;
        sEntityChanged = true;
      }
      else
        sEntityChanged = false;

      if (currentEntity.HasComponent<Component::Tag>())
        TagComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Layer>())
        LayerComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Transform>())
        TransformComponentWindow(currentEntity);
    }

    ImGui::End();
  }

  bool const Inspector::GetIsComponentEdited() const {
    return sIsComponentEdited;
  }

  void Inspector::SetIsComponentEdited(bool isComponentEdited) {
    sIsComponentEdited = isComponentEdited;
  }

  void Inspector::TagComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin("Tag") };

    if (isOpen) {
      std::string tag{ entity.GetTag() };
      if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.SetTag(tag);
        SetIsComponentEdited(true);
      }
    }

    WindowEnd(isOpen);
  }

  void Inspector::LayerComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin("Layer") };

    if (isOpen) {
      
    }

    WindowEnd(isOpen);
  }

  void Inspector::TransformComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin("Transform") };

    if (isOpen) {

    }

    WindowEnd(isOpen);
  }

  bool Inspector::WindowBegin(std::string windowName) {
    ImGui::Separator();

    if (sEntityChanged) {
      bool& openMapStatus = sComponentOpenStatusMap[windowName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    bool isOpen{ ImGui::TreeNode(windowName.c_str()) };
    sComponentOpenStatusMap[windowName] = isOpen;
    return isOpen;
  }

  void Inspector::WindowEnd(bool isOpen) {
    if (isOpen)
      ImGui::TreePop();

    ImGui::Separator();
  }
} // namespace GUI
#include <pch.h>
#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Inspector.h"

namespace GUI {
  Inspector::Inspector(std::string const& name) : GUIWindow(name) {}

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());

    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    if (currentEntity) {
      if (currentEntity.HasComponent<Component::Tag>()) {
        TagComponentWindow(currentEntity);
      }

      if (currentEntity.HasComponent<Component::Layer>()) {
        LayerComponentWindow(currentEntity);
      }

      if (currentEntity.HasComponent<Component::Transform>()) {
        TransformComponentWindow(currentEntity);
      }
    }

    ImGui::End();
  }

  void Inspector::TagComponentWindow(ECS::Entity entity) {
    ImGui::Separator();
    std::string tag{ entity.GetTag() };
    if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
      entity.SetTag(tag);
    }
    ImGui::Separator();
  }

  void Inspector::LayerComponentWindow(ECS::Entity entity) {
    ImGui::Separator();

    ImGui::Separator();
  }

  void Inspector::TransformComponentWindow(ECS::Entity entity) {
    ImGui::Separator();

    ImGui::Separator();
  }
} // namespace GUI

#include "Layers.h"
#include <ImGui/imgui.h>
#include "Core/Systems/LayerSystem/LayerSystem.h"

namespace GUI {

  Layers::Layers(const char* name) : GUIWindow(name) {

  }

  void Layers::Run() {
    ImGui::Begin(mWindowName.c_str());

    LayerNameNode();
    VisibilityToggleNode();
    CollisionMatrixNode();

    ImGui::End();
  }

  void Layers::LayerNameNode() {

  }

  void Layers::VisibilityToggleNode() {
    if (ImGui::TreeNodeEx("Visibility##", ImGuiTreeNodeFlags_DefaultOpen)) {
      for (int i{}; i < Systems::MAX_LAYERS; ++i) {
        if (Systems::LayerSystem::GetLayerNames()[i] == "") continue;
        std::string nameText = "Layer " + std::to_string(i) + ": " + Systems::LayerSystem::GetLayerNames()[i];
        ImGui::Text(nameText.c_str());
        ImGui::SameLine();
        ImGui::SetCursorPosX(SAME_LINE_SPACING);
        ImGui::Checkbox(("##readonly" + std::to_string(i)).c_str(), 
          reinterpret_cast<bool*>(&Systems::LayerSystem::GetLayerVisibility()[i]));
      }

      ImGui::TreePop();
    }
  }

  void Layers::CollisionMatrixNode() {

  }

}
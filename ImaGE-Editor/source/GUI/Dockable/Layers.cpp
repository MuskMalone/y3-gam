#include <pch.h>
#include "Layers.h"
#include <ImGui/imgui.h>
#include <Core/Systems/SystemManager/SystemManager.h>

namespace GUI {

  Layers::Layers(const char* name) : GUIWindow(name) {
    mLayerSystem = Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>();
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
      if (std::shared_ptr<Systems::LayerSystem> layerSys = mLayerSystem.lock()) {
        auto const& layerNames{ layerSys->GetLayerNames() };

        for (int i{}; i < Systems::MAX_LAYERS; ++i) {
          if (layerNames[i] == "") { continue; }

          std::string nameText = "Layer " + std::to_string(i) + ": " + layerNames[i];
          ImGui::Text(nameText.c_str());
          ImGui::SameLine();
          ImGui::SetCursorPosX(SAME_LINE_SPACING);
          ImGui::Checkbox(("##readonly" + std::to_string(i)).c_str(),
            reinterpret_cast<bool*>(&layerSys->GetLayerVisibility()[i]));
        }
      }
      else {
        Debug::DebugLogger::GetInstance().LogCritical("[Layers] LayerSystem no longer exists! Skipping update.");
      }
      ImGui::TreePop();
    }
  }

  void Layers::CollisionMatrixNode() {

  }

}
#include <pch.h>
#include "Layers.h"
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include "GUI/GUIManager.h"

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
    ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    if (ImGui::TreeNodeEx("Layer Names##", ImGuiTreeNodeFlags_DefaultOpen)) {
      if (std::shared_ptr<Systems::LayerSystem> layerSys = mLayerSystem.lock()) {

        float contentSize = ImGui::GetContentRegionAvail().x;
        float charSize = ImGui::CalcTextSize("012345678901234").x;
        float inputWidth = (contentSize - charSize - 60);
        ImGui::BeginTable("LayerNameTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, 100.f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

        for (int i{}; i < Systems::MAX_LAYERS; ++i) {
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);

          std::string layerName{ layerSys->GetLayerNames()[i] };
          std::string layerNameText = "Layer " + std::to_string(i);
          
          ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_REGULAR));
          ImGui::Text(layerNameText.c_str());
          ImGui::PopFont();
          
          ImGui::TableSetColumnIndex(1);
          if (i < Systems::MAX_BUILTIN_LAYERS) {
            ImGui::BeginDisabled();
          }

          ImGui::SetNextItemWidth(INPUT_SIZE);
          ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_LIGHT));
          if (ImGui::InputText("##readonly", &layerName, ImGuiInputTextFlags_EnterReturnsTrue)) {
            layerSys->SetLayerName(i, layerName);
          }
          ImGui::PopFont();

          if (i < Systems::MAX_BUILTIN_LAYERS) {
            ImGui::EndDisabled();
          }
        }

        ImGui::EndTable();
      }

      else {
        Debug::DebugLogger::GetInstance().LogCritical("[Layers] LayerSystem no longer exists! Skipping update.");
      }

      ImGui::TreePop();
    }
    ImGui::PopFont();
  }

  void Layers::VisibilityToggleNode() {
    ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));

    if (ImGui::TreeNodeEx("Visibility##", ImGuiTreeNodeFlags_DefaultOpen)) {
      if (std::shared_ptr<Systems::LayerSystem> layerSys = mLayerSystem.lock()) {
        auto const& layerNames{ layerSys->GetLayerNames() };

        float contentSize = ImGui::GetContentRegionAvail().x;
        float charSize = ImGui::CalcTextSize("012345678901234").x;
        float inputWidth = (contentSize - charSize - 60);

        ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_LIGHT));
        ImGui::BeginTable("VisibilityTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, contentSize / 2.f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

        for (int i{}; i < Systems::MAX_LAYERS; i += 2) {
          //if (layerNames[i] == "") { continue; }

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);

          ImGui::Checkbox(("##readonly" + std::to_string(i)).c_str(),
            reinterpret_cast<bool*>(&layerSys->GetLayerVisibility()[i]));
          ImGui::SameLine();
          ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_REGULAR));
          std::string nameText = "Layer " + std::to_string(i) + ": ";
          ImGui::Text(nameText.c_str());
          ImGui::PopFont();
          ImGui::SameLine();
          ImGui::Text(layerNames[i].c_str());

          ImGui::TableSetColumnIndex(1);
          ImGui::Checkbox(("##readonly" + std::to_string(i + 1)).c_str(),
            reinterpret_cast<bool*>(&layerSys->GetLayerVisibility()[i + 1]));
          ImGui::SameLine();
          ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_REGULAR));
          std::string secondNameText = "Layer " + std::to_string(i + 1) + ": ";
          ImGui::Text(secondNameText.c_str());
          ImGui::PopFont();
          ImGui::SameLine();
          ImGui::Text(layerNames[i + 1].c_str());
        }

        ImGui::EndTable();
        ImGui::PopFont();
      }

      else {
        Debug::DebugLogger::GetInstance().LogCritical("[Layers] LayerSystem no longer exists! Skipping update.");
      }

      ImGui::TreePop();
    }

    ImGui::PopFont();
  }

  void Layers::CollisionMatrixNode() {
    ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    if (ImGui::TreeNodeEx("Collision Matrix##", ImGuiTreeNodeFlags_DefaultOpen)) {
      if (std::shared_ptr<Systems::LayerSystem> layerSys = mLayerSystem.lock()) {

      }

      else {
        Debug::DebugLogger::GetInstance().LogCritical("[Layers] LayerSystem no longer exists! Skipping update.");
      }

      ImGui::TreePop();
    }
    ImGui::PopFont();
  }

}
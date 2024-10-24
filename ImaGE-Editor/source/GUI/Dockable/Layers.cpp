#include <pch.h>
#include "Layers.h"
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include "GUI/GUIManager.h"

namespace GUI {

  int Layers::sSelectedLayer{};

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
    if (ImGui::TreeNodeEx("Layer Names##")) {
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
          std::string inputTextString = "##inputTextReadOnly" + i;
          if (ImGui::InputText(inputTextString.c_str(), &layerName, ImGuiInputTextFlags_EnterReturnsTrue)) {
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

    if (ImGui::TreeNodeEx("Visibility##")) {
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
    if (ImGui::TreeNodeEx("Collision##")) {
      if (std::shared_ptr<Systems::LayerSystem> layerSys = mLayerSystem.lock()) {

        auto const& layerNames{ layerSys->GetLayerNames() };
        float contentSize = ImGui::GetContentRegionAvail().x;
        float charSize = ImGui::CalcTextSize("012345678901234").x;
        float inputWidth = (contentSize - charSize - 60);

        ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_LIGHT));
        ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_REGULAR));
        ImGui::Text("Layer Selection");
        ImGui::PopFont();
        ImGui::BeginTable("CollisionMatrixLayerSelectionTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

        for (int i{}; i < Systems::MAX_LAYERS; i += 4) {
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          std::string buttonText1 = "Layer " + std::to_string(i) + "##FirstButton";
          bool colorPushed = false;
          if (sSelectedLayer == i) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.2f, 0.6f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText1.c_str())) {
            sSelectedLayer = i;
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
          ImGui::SameLine();

          ImGui::TableSetColumnIndex(1);
          std::string buttonText2 = "Layer " + std::to_string(i + 1) + "##SecondButton";
          colorPushed = false;
          if (sSelectedLayer == i + 1) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.2f, 0.6f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText2.c_str())) {
            sSelectedLayer = i + 1;
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i + 1] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i + 1] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
          ImGui::SameLine();

          ImGui::TableSetColumnIndex(2);
          std::string buttonText3 = "Layer " + std::to_string(i + 2) + "##ThirdButton";
          colorPushed = false;
          if (sSelectedLayer == i + 2) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.2f, 0.6f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText3.c_str())) {
            sSelectedLayer = i + 2;
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i + 2] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i + 2] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
          ImGui::SameLine();

          ImGui::TableSetColumnIndex(3);
          std::string buttonText4 = "Layer " + std::to_string(i + 3) + "##FourthButton";
          colorPushed = false;
          if (sSelectedLayer == i + 3) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.2f, 0.6f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText4.c_str())) {
            sSelectedLayer = i + 3;
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i + 3] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i + 3] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
        }

        ImGui::EndTable();

        std::string selectedLayerText{ "Selected Layer " + std::to_string(sSelectedLayer) + ": " + 
          layerSys->GetLayerNames()[sSelectedLayer] };

        ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_REGULAR));
        ImGui::Text(selectedLayerText.c_str());
        ImGui::PopFont();

        ImGui::BeginTable("CollisionMatrixLayerListTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));

        for (int i{}; i < Systems::MAX_LAYERS; i += 4) {
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          std::string buttonText1 = "Layer " + std::to_string(i) + "##FirstColButton";
          bool colorPushed = false;
          if (static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i]) == true) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText1.c_str())) {
            layerSys->SetLayerCollisionList(sSelectedLayer, i,
              !static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i]));
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
          ImGui::SameLine();

          ImGui::TableSetColumnIndex(1);
          std::string buttonText2 = "Layer " + std::to_string(i + 1) + "##SecondColButton";
          colorPushed = false;
          if (static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i + 1]) == true) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText2.c_str())) {
            layerSys->SetLayerCollisionList(sSelectedLayer, i + 1, 
              !static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i + 1]));
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i + 1] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i + 1] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
          ImGui::SameLine();

          ImGui::TableSetColumnIndex(2);
          std::string buttonText3 = "Layer " + std::to_string(i + 2) + "##ThirdColButton";
          colorPushed = false;
          if (static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i + 2]) == true) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText3.c_str())) {
            layerSys->SetLayerCollisionList(sSelectedLayer, i + 2,
              !static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i + 2]));
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i + 2] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i + 2] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
          ImGui::SameLine();

          ImGui::TableSetColumnIndex(3);
          std::string buttonText4 = "Layer " + std::to_string(i + 3) + "##FourthColButton";
          colorPushed = false;
          if (static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i + 3]) == true) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));
            colorPushed = true;
          }
          if (ImGui::Button(buttonText4.c_str())) {
            layerSys->SetLayerCollisionList(sSelectedLayer, i + 3,
              !static_cast<bool>(layerSys->GetLayerCollisionList(sSelectedLayer)[i + 3]));
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string hoverToolTip{ layerNames[i + 3] == "" ? "Name: Layer does not have a Name!" : "Name: "
              + layerNames[i + 3] };
            ImGui::Text(hoverToolTip.c_str());
            ImGui::EndTooltip();
          }
          if (colorPushed) {
            ImGui::PopStyleColor(3);
          }
        }

        ImGui::PopStyleColor(3);
        ImGui::EndTable();

        ImGui::PopFont();

        ImGui::PushFont(GUIManager::GetStyler().GetCustomFont(GUI::MONTSERRAT_REGULAR));
        if (ImGui::Button("Collide With All")) {
          for (int i{}; i < Systems::MAX_LAYERS; ++i) {
            layerSys->SetLayerCollisionList(sSelectedLayer, i, true);
          }
        }
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::Text("Make Layer collide with every other Layer");
          ImGui::EndTooltip();
        }

        if (ImGui::Button("Collide With None")) {
          for (int i{}; i < Systems::MAX_LAYERS; ++i) {
            layerSys->SetLayerCollisionList(sSelectedLayer, i, false);
          }
        }
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::Text("Make Layer NOT collide with every other Layer");
          ImGui::EndTooltip();
        }
        ImGui::PopFont();
      }

      else {
        Debug::DebugLogger::GetInstance().LogCritical("[Layers] LayerSystem no longer exists! Skipping update.");
      }

      ImGui::TreePop();
    }

    ImGui::PopFont();
  }

}
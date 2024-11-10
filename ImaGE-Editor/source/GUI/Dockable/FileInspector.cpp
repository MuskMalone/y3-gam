/*!*********************************************************************
\file   FileInspector.cpp
\author
\date   5-October-2024
\brief  Class encapsulating functions to run the inspector / property
        window of the editor. Displays and allows modification of
        components for the currently selected file.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Inspector.h"
#include <Asset/IGEAssets.h>
#include <Graphics/MaterialData.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Graphics/MaterialTable.h>

namespace {
  static std::filesystem::path prevMaterial;
}

namespace GUI {

  void Inspector::RunFileInspector() {
    std::filesystem::path const selectedFile{ GUIVault::GetSelectedFile() };
    
    // @TODO: Work for other files in future
    if (selectedFile.empty() || selectedFile.extension() != gMaterialFileExt) { return; }


    MaterialInspector(selectedFile);

    ImGui::PopFont();
  }

  void Inspector::MaterialInspector(std::filesystem::path const& selectedFile) {
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    auto& selectedMaterial{ am.GetAsset<IGE::Assets::MaterialAsset>(am.LoadRef<IGE::Assets::MaterialAsset>(selectedFile.string()))->mMaterial };

    if (!prevMaterial.empty() && prevMaterial != selectedFile && std::filesystem::exists(prevMaterial)) {
      Graphics::MaterialTable::CreateAndImportMatFile(prevMaterial.stem().string());
    }
    prevMaterial = selectedFile;

    /*{
      std::string name{ selectedMaterial->GetName() };
      if (ImGui::InputText("##MatName", &name)) {
        selectedMaterial->SetName(name);
      }
    }*/
    ImGui::Text(selectedMaterial->GetName().c_str());
    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_LIGHT));

    float const inputWidth{ CalcInputWidth(60.f) };
    if (ImGui::BeginTable("MaterialTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);


      NextRowTable("Albedo");
      {
        glm::vec3 albedo{ selectedMaterial->GetAlbedoColor() };
        if (ImGui::ColorEdit4("##AlbedoCol", &albedo[0], ImGuiColorEditFlags_NoAlpha)) {
          selectedMaterial->SetAlbedoColor(albedo);
        }
      }

      NextRowTable("Metalness");
      {
        float metalness{ selectedMaterial->GetMetalness() };
        if (ImGui::SliderFloat("##Metalness", &metalness, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
          selectedMaterial->SetMetalness(metalness);
        }
      }

      NextRowTable("Roughness");
      {
        float roughness{ selectedMaterial->GetRoughness() };
        if (ImGui::SliderFloat("##Roughness", &roughness, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
          selectedMaterial->SetRoughness(roughness);
        }
      }

      NextRowTable("Emission");
      {
        float emission{ selectedMaterial->GetEmission() };
        if (ImGui::SliderFloat("##Emission", &emission, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
          selectedMaterial->SetEmission(emission);
        }
      }

      NextRowTable("Transparency");
      {
        float transparency{ selectedMaterial->GetTransparency() };
        if (ImGui::SliderFloat("##Transparency", &transparency, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
          selectedMaterial->SetTransparency(transparency);
        }
      }

      NextRowTable("Ambient Occlusion");
      {
        float ao{ selectedMaterial->GetAO() };
        if (ImGui::SliderFloat("##AO", &ao, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
          selectedMaterial->SetAO(ao);
        }
      }

      ImGui::EndTable();
    }

    ImGui::NewLine();
    if (ImGui::BeginTable("MaterialVec2Table", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      float const vec2InputWidth{ inputWidth * 0.5f };
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("  X", ImGuiTableColumnFlags_WidthFixed, vec2InputWidth);
      ImGui::TableSetupColumn("  Y", ImGuiTableColumnFlags_WidthFixed, vec2InputWidth);
      ImGui::TableHeadersRow();

      {
        glm::vec2 tiling{ selectedMaterial->GetTiling() };
        if (ImGuiHelpers::TableInputFloat2("Tiling", &tiling[0], vec2InputWidth, false, -FLT_MAX, FLT_MAX, -0.03f)) {
          selectedMaterial->SetTiling(tiling);
        }
      }

      {
        glm::vec2 offset{ selectedMaterial->GetOffset() };
        if (ImGuiHelpers::TableInputFloat2("Tiling", &offset[0], vec2InputWidth, false, -FLT_MAX, FLT_MAX, -0.03f)) {
          selectedMaterial->SetOffset(offset);
        }
      }

      ImGui::EndTable();
    }
  }

} // namespace GUI

namespace {
  
}

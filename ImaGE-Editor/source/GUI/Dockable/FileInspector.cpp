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
#include <GUI/Helpers/AssetPayload.h>

namespace {
  static IGE::Assets::GUID sPrevFile{};

  void NextRowTable(const char* labelName);
  std::string DragDropComponent(GUI::AssetPayload::AssetType type);
}

namespace GUI {

  void Inspector::RunFileInspector() {
    std::filesystem::path const selectedFile{ GUIVault::GetSelectedFile() };
    
    // @TODO: Work for other files in future
    if (selectedFile.empty() || selectedFile.extension() != gMaterialFileExt) { return; }


    MaterialInspector(selectedFile);

    ImGui::PopFont();
  }

  #define TextureMapField(Title, MapType) {NextRowTable(Title);\
            if (selectedMaterial->IsDefault##MapType##Map()) {\
              name = "Drag Texture Here##" Title;\
            } else {\
               IGE::Assets::GUID guid{ selectedMaterial->Get##MapType##Map() };\
                am.LoadRef<IGE::Assets::TextureAsset>(guid);\
                name = am.GUIDToPath(guid).c_str();\
            }\
            if (ImGui::Button(name.c_str(), ImVec2(inputWidth, 30.f))) { selectedMaterial->Set##MapType##Map({}); }\
            if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Remove Texture"); }\
            std::string const path{ DragDropComponent(AssetPayload::SPRITE) };\
            if (!path.empty()) { try {\
              selectedMaterial->Set##MapType##Map(am.LoadRef<IGE::Assets::TextureAsset>(path));\
            } catch (Debug::ExceptionBase& e) { e.LogSource(); } } }

  void Inspector::MaterialInspector(std::filesystem::path const& selectedFile) {
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    IGE::Assets::GUID guid{};

    try {
      guid = am.LoadRef<IGE::Assets::MaterialAsset>(selectedFile.string());
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("Unable to load Material window for " + selectedFile.string());
      GUIVault::SetSelectedFile({});
      return;
    }

    auto& selectedMaterial{ am.GetAsset<IGE::Assets::MaterialAsset>(guid)->mMaterial };

    if (sPrevFile != guid) {
      SaveLastEditedFile();
      sPrevFile = guid;
    }

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
        glm::vec4 emission{ selectedMaterial->GetEmission() };
        if (ImGui::ColorEdit4("##Emission", &emission[0], ImGuiColorEditFlags_NoAlpha)) {
          selectedMaterial->SetEmission(emission);
        }
        if (ImGui::SliderFloat("##EmissionBrightness", &emission[3], 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
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
        if (ImGuiHelpers::TableInputFloat2("Offset", &offset[0], vec2InputWidth, false, -FLT_MAX, FLT_MAX, -0.03f)) {
          selectedMaterial->SetOffset(offset);
        }
      }

      ImGui::EndTable();
    }

    ImGui::NewLine();
    // Shader Dropdown
    ImGui::Text("Shader");
    static const char* shaderOptions[] = { "PBR", "Unlit", "Water", "Leaf" }; // Added "Water" option
    static int currentShaderIndex = (selectedMaterial->GetShaderName() == "Unlit") ? 1 :
        (selectedMaterial->GetShaderName() == "Water") ? 2 : (selectedMaterial->GetShaderName() == "Leaf") ? 3: 0;

    if (ImGui::Combo("##ShaderDropdown", &currentShaderIndex, shaderOptions, IM_ARRAYSIZE(shaderOptions))) {
        selectedMaterial->SetShaderName(shaderOptions[currentShaderIndex]);
    }


    ImGui::NewLine();
    ImGui::Text("Textures");
    if (ImGui::BeginTable("MaterialTable2", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
      std::string name;

      TextureMapField("Albedo Map", Albedo);
      TextureMapField("Normal Map", Normal);
      TextureMapField("Metalness Map", Metalness);
      TextureMapField("Roughness Map", Roughness);

      ImGui::EndTable();
    }
  }

  void Inspector::SaveLastEditedFile() const {
    if (sPrevFile) {
      Graphics::MaterialTable::SaveMaterial(sPrevFile);
    }
  }

} // namespace GUI

namespace {
  void NextRowTable(const char* labelName) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(labelName);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(GUI::Inspector::INPUT_SIZE);
  }

  std::string DragDropComponent(GUI::AssetPayload::AssetType type) {
    if (ImGui::BeginDragDropTarget()) {
      ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(GUI::AssetPayload::sAssetDragDropPayload);
      if (drop) {
        GUI::AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
        if (assetPayload.mAssetType == type) {
          return assetPayload.GetFilePath().c_str();
        }
      }
      ImGui::EndDragDropTarget();
    }

    return {};
  }
}

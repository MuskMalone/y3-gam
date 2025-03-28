#include <pch.h>
#include "PostProcessingSettings.h"
#include <GUI/Helpers/AssetPayload.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <imgui/imgui.h>

#include "Asset/AssetManager.h"
#include "Graphics/PostProcessing/PostProcessingManager.h"
#include <Scenes/SceneManager.h>

namespace {
  std::string GetFileContents(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
      Debug::DebugLogger::GetInstance().LogWarning("couldnt open shader");
      return "error reading shader!";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
}

namespace GUI {
  PostProcessingSettings::PostProcessingSettings(const char* name) : GUIWindow{ name },
    mPostProcessingMgr{ Graphics::PostProcessingManager::GetInstance() }
  {
  }

  void PostProcessingSettings::Run()
  {
    ImGui::Begin(mWindowName.c_str());
    {
      Scenes::SceneManager& sceneManager{ IGE_SCENEMGR };
      if (sceneManager.NoSceneSelected() || sceneManager.GetSceneState() == Scenes::PREFAB_EDITOR) {
        ImGui::End();
        return; 
      }
    }

    {
      // Text label for the fog settings
      ImGui::Text("Fog Settings");

      auto& mindist{ mPostProcessingMgr.GetFogMinDist() };
      // Slider for mMinDist
      ImGui::SliderFloat("Min Distance", &mindist, 0.0f, 1000.f, "Min: %.1f");
      auto& maxdist{ mPostProcessingMgr.GetFogMaxDist() };
      // Slider for mMaxDist
      ImGui::SliderFloat("Max Distance", &maxdist, 0.0f, 1000.f, "Max: %.1f");
      auto& color{ mPostProcessingMgr.GetFogColor() };
      // Color picker for mFogColor
      ImGui::ColorEdit3("Fog Color", &color.r); // RGB picker

      // Add a partition line (separator)
      ImGui::Separator();
    }
    {
      // Set the size of the drop box
      ImVec2 boxSize(200.0f, 50.0f);
      ImVec2 cursorPos = ImGui::GetCursorScreenPos();
      ImVec2 boxEnd = ImVec2(cursorPos.x + boxSize.x, cursorPos.y + boxSize.y);

      // Draw the text box with a border
      ImGui::BeginChild("ShaderDropBox", boxSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

      // Center the text
      ImVec2 textSize = ImGui::CalcTextSize("Drag here to add shader");
      ImVec2 textPos = ImVec2(cursorPos.x + (boxSize.x - textSize.x) * 0.5f, cursorPos.y + (boxSize.y - textSize.y) * 0.5f);
      ImGui::SetCursorScreenPos(textPos);
      ImGui::TextUnformatted("Drag here to add shader");
      ShaderDragDropEvent();
      ImGui::EndChild();
      //if (!fileContent.empty()) {
      //    fileContent.clear();
      //}
    }
    auto numShaders{ mPostProcessingMgr.GetShaderNum() };
    for (unsigned i{}; i < numShaders; ++i) {
      static std::unordered_map<std::string, std::string> fileContents;

      // Load each file's content only once
      for (unsigned j{}; j < numShaders; ++j) {
        std::string path{ mPostProcessingMgr.GetShaderName(j) };
        if (fileContents.find(path) == fileContents.end()) {
          fileContents.emplace(path, GetFileContents(path));
        }
      }

      // Display file name with Refresh and Delete buttons next to it
      auto fileName{ mPostProcessingMgr.GetShaderName(i) };
      ImGui::TextUnformatted(fileName.c_str());
      ImGui::SameLine();

      if (ImGui::Button(("Refresh##" + std::to_string(i)).c_str())) {
        // Reload the file content 
        mPostProcessingMgr.ReloadShader(i);
        fileContents[fileName] = std::move(GetFileContents(fileName));
      }

      ImGui::SameLine();

      // Delete Button
      if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
        mPostProcessingMgr.RemoveShader(i);
      }

      // Display the text editor
      ImGui::Separator();
      ImGui::BeginChild(("FileContent##" + std::to_string(i)).c_str(), ImVec2(0, ImGui::GetTextLineHeight() * 20), true, ImGuiWindowFlags_HorizontalScrollbar);
      ImGui::TextWrapped("%s", fileContents[fileName].c_str());  // Display content with wrapped text
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
      {
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
          AssetHelpers::OpenFileWithDefaultProgram(fileName);
        }
      }
      ShaderDragDropEvent();
      ImGui::EndChild();
    }
    ImGui::End();
  }

  void PostProcessingSettings::ShaderDragDropEvent() {
    // Handle drag-and-drop target
    if (ImGui::BeginDragDropTarget()) {
      ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
      if (drop) {
        AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
        if (assetPayload.mAssetType == AssetPayload::SHADER) {
          auto fp{ assetPayload.GetFilePath() };
          try {
            mPostProcessingMgr.SetShader(
              IGE_ASSETMGR.PathToGUID(fp)
            );
          }
          catch ([[maybe_unused]] Debug::Exception<IGE::Assets::AssetManager> const& e) {
            //do nothing
            Debug::DebugLogger::GetInstance().LogWarning("shader asset doesnt exist");
          }
        }
      }
      ImGui::EndDragDropTarget();
    }
  }
} // namespace GUI

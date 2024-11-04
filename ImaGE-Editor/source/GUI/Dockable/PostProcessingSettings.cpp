#include <pch.h>
#include "PostProcessingSettings.h"
#include "Graphics/PostProcessing/PostProcessingManager.h"
#include <GUI/Helpers/AssetPayload.h>
#include <GUI/Helpers/AssetHelpers.h>
#include "Asset/AssetManager.h"
#include <imgui/imgui.h>
namespace GUI {
	PostProcessingSettings::PostProcessingSettings(const char* name) : GUIWindow{name}
	{
	}
    void ShaderDragDropEvent() {
        // Handle drag-and-drop target
        if (ImGui::BeginDragDropTarget()) {
            ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
            if (drop) {
                AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
                if (assetPayload.mAssetType == AssetPayload::SHADER) {
                    auto fp{ assetPayload.GetFilePath() };
                    try {
                        Graphics::PostProcessingManager::GetInstance().SetShader(
                            IGE_ASSETMGR.PathToGUID(fp)
                        );
                    }
                    catch (Debug::Exception<IGE::Assets::AssetManager> const& e) {
                        //do nothing
                        Debug::DebugLogger::GetInstance().LogWarning("shader asset doesnt exist");
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
	void PostProcessingSettings::Run()
	{
        std::string fileContent = "";
        ImGui::Begin("Post Processing");
        if (Graphics::PostProcessingManager::GetInstance().GetShaderName().empty()) {
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
            if (!fileContent.empty()) {
                fileContent.clear();
            }
        }
        else {

            
            // Display file name with Refresh and Delete buttons next to it
            auto fileName{ Graphics::PostProcessingManager::GetInstance().GetShaderName()};
            ImGui::TextUnformatted(fileName.c_str());
            ImGui::SameLine();

            // Refresh Button
            if (fileContent.empty()) {
                std::ifstream file(fileName);
                if (file) {
                    std::ostringstream ss;
                    ss << file.rdbuf();
                    fileContent = ss.str();  // Load file content into fileContent string
                }
            }
            if (ImGui::Button("Refresh")) {
                // Reload the file content 
                Graphics::PostProcessingManager::GetInstance().ReloadShader();
                std::ifstream file(fileName);
                if (file) {
                    std::ostringstream ss;
                    ss << file.rdbuf();
                    fileContent = ss.str();  // Load file content into fileContent string
                }
            }

            ImGui::SameLine();

            // Delete Button
            if (ImGui::Button("Delete")) {
                Graphics::PostProcessingManager::GetInstance().RemoveShader();
            }

            // Display the text editor
            ImGui::Separator();
            ImGui::BeginChild("FileContent", ImVec2(0, ImGui::GetTextLineHeight() * 20), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextWrapped("%s", fileContent.c_str());  // Display content with wrapped text
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
}
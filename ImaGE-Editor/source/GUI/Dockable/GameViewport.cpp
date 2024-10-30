#include <pch.h>
#include "GameViewport.h"
#include <imgui/imgui.h>
#include <Graphics/RenderPass/GeomPass.h>

namespace GUI {

    GameViewport::GameViewport(const char* name)
        : GUIWindow(name) {}

    void GameViewport::Run() {
        //ImGui::Begin(mWindowName.c_str());

        //ImVec2 const vpSize = ImGui::GetContentRegionAvail();
        //auto const finalPass{ Graphics::Renderer::GetPass<Graphics::GeomPass>() };

        //// Render the framebuffer as an image
        //ImGui::Image(
        //    reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(finalPass->GetTargetFramebuffer()->GetColorAttachmentID())),
        //    vpSize,
        //    ImVec2(0, 1),
        //    ImVec2(1, 0)
        //);

        //// Add input handling for the game view if necessary
        //ProcessCameraInputs();

        //ImGui::End();
    }

    void GameViewport::Render(std::shared_ptr<Graphics::Texture> const& tex)
    {
        ImGui::Begin(mWindowName.c_str());

        ImVec2 const vpSize = ImGui::GetContentRegionAvail();
        auto const finalPass{ Graphics::Renderer::GetPass<Graphics::GeomPass>() };

        // Render the framebuffer as an image
        ImGui::Image(
            reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(tex->GetTexHdl())),
            vpSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        ImGui::End();
        
    }

    //void GameViewport::ProcessCameraInputs() {
        // Implement input handling for camera movement here if necessary
    //}

} // namespace GUI

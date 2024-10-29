#include <pch.h>
#include "RenderPassViewer.h"
#include <Graphics/Renderer.h>
#include <Graphics/RenderPass/ShadowPass.h>

namespace GUI {

  RenderPassViewer::RenderPassViewer(const char* windowName) : GUIWindow(windowName) {

  }

  void RenderPassViewer::Run() {
    ImGui::Begin(mWindowName.c_str());

    {
      auto const shadowPass{ Graphics::Renderer::GetPass<Graphics::ShadowPass>() };
      ImGui::Text(shadowPass->GetSpecification().debugName.c_str());

      if (shadowPass->IsActive()) {
        uint32_t colorBufferId{ shadowPass->GetTargetFramebuffer()->GetColorAttachmentID() };
        ImGui::Image(
          reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(colorBufferId)),
          ImGui::GetContentRegionAvail(),
          ImVec2(0, 1),
          ImVec2(1, 0)
        );
      }
      else {
        ImGui::Text("Nothing to show. Add a light that casts shadows to the scene!");
      }
    }

    ImGui::End();
  }

} // namespace GUI

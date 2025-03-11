#include <pch.h>
#include "GameViewport.h"
#include <imgui/imgui.h>
#include <Graphics/RenderPass/GeomPass.h>
#include "Graphics/Renderer.h"
#include <Events/EventManager.h>
#include <EditorEvents.h>

namespace GUI {

    GameViewport::GameViewport(const char* name) : GUIWindow(name),
      mFocusWindow{ false } {
      SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &GameViewport::OnScenePlay, this);
    }

    void GameViewport::Render(std::shared_ptr<Graphics::Texture> const& tex)
    {
        ImGui::Begin(mWindowName.c_str());

        if (!tex) {
          ImGui::Text("Scene has no Camera component");
          ImGui::End();
          return;
        }

        ImVec2 const vpSize = ImGui::GetContentRegionAvail();
        auto const finalPass{ Graphics::Renderer::GetPass<Graphics::GeomPass>() };

        // Render the framebuffer as an image
        ImGui::Image(
            reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(tex->GetTexHdl())),
            vpSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        // set focus to game vp if scene started for the first time
        if (mFocusWindow) {
          ImGui::SetWindowFocus();
          mFocusWindow = false;
        }

        if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_K)) {
          QUEUE_EVENT(Events::ToggleImGui);
        }

        ImGui::End();
        
    }

    EVENT_CALLBACK_DEF(GameViewport, OnScenePlay) {
      // set flag if scene started
      if (CAST_TO_EVENT(Events::SceneStateChange)->mNewState == Events::SceneStateChange::STARTED) {
        mFocusWindow = true;
      }
    }

} // namespace GUI

#include <pch.h>
#ifndef IMGUI_DISABLE
#include "Viewport.h"
#include <imgui/imgui.h>
#include <GUI/Helpers/AssetPayload.h>
#include "AssetBrowser.h"
#include <Events/EventManager.h>

namespace GUI
{

  Viewport::Viewport(std::string const& name, Graphics::Framebuffer const& framebuffer) : mFramebuffer{ framebuffer }, GUIWindow(name) {}

  void Viewport::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    ImGui::Image(
      (ImTextureID)(uintptr_t)mFramebuffer.GetTextureID(),
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1),
      ImVec2(1, 0)
    );

    ReceivePayload();

    ImGui::End();
  }

  void Viewport::ReceivePayload()
  {
    if (ImGui::BeginDragDropTarget())
    {
      ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetBrowser::sAssetDragDropPayload);
      if (drop)
      {
        AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
        switch (assetPayload.mAssetType)
        {
        case AssetPayload::SCENE:
          QUEUE_EVENT(Events::LoadSceneEvent, assetPayload.GetFileName(), assetPayload.GetFilePath());
          break;
        case AssetPayload::PREFAB:
          // @TODO: Convert screen to world pos when viewport is up
          glm::vec3 const prefabPos{};
          QUEUE_EVENT(Events::SpawnPrefabEvent, assetPayload.GetFileName(), assetPayload.GetFilePath(), prefabPos);
          break;
        case AssetPayload::SPRITE:

          break;
        case AssetPayload::AUDIO:

          break;
        default: break;
        }
      }
      ImGui::EndDragDropTarget();
    }
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

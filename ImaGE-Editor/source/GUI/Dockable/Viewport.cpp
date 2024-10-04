#include <pch.h>
#include "Viewport.h"
#include <imgui/imgui.h>
#include <GUI/Helpers/AssetPayload.h>
#include "AssetBrowser.h"
#include <Events/EventManager.h>
#include <FrameRateController/FrameRateController.h>
#include <Core/Entity.h>
#include <Core/Components/Mesh.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>
#include <GUI/Helpers/ImGuiHelpers.h>

namespace GUI
{

  Viewport::Viewport(std::string const& name) : GUIWindow(name) {}

  void Viewport::Render(Graphics::RenderTarget& renderTarget)
  {
    ImGui::Begin(mWindowName.c_str());

    ImVec2 const startCursorPos{ ImGui::GetCursorPos() };

    // only register input if viewport is focused
    if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered()) {
      ProcessCameraInputs(renderTarget.scene.GetEditorCamera());
    }
    // auto focus window when middle or right-clicked upon
    else if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle))) {
      ImGui::FocusWindow(ImGui::GetCurrentWindow());
    }


    // update framebuffer
    ImGui::Image(
      reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(renderTarget.framebuffer->GetColorAttachmentID())),
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1),
      ImVec2(1, 0)
    );

    ReceivePayload();

    ImGui::SetCursorPos(startCursorPos);
    ImGui::Text("Middle Click - Pan");
    ImGui::Text("Scroll - Zoom");
    ImGui::Text("While Right-click Held:");
    ImGui::Text("       Left Click - Look");
    ImGui::Text("       WASDQE - Move");

    ImGui::End();
  }

  void Viewport::ProcessCameraInputs(Graphics::EditorCamera& cam) {
    static bool isDragging{ false }, isPanning{ false };
    static ImVec2 previousMousePos;
    using enum Graphics::EditorCamera::CameraMovement;  // C++20 <3
    float const dt{ Performance::FrameRateController::GetInstance().GetDeltaTime() };

    ImVec2 const windowSize{ ImGui::GetContentRegionAvail() };

    // only allow movement and panning if right-click held
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
      if (!isPanning && !isDragging) {
        previousMousePos = ImGui::GetMousePos();
        isDragging = true;
      }
      ImGui::SetMouseCursor(ImGuiMouseCursor_None); // hide cursor when looking

      // process input for movement
      if (ImGui::IsKeyDown(ImGuiKey_W)) {
        cam.ProcessKeyboardInput(FORWARD, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_S)) {
        cam.ProcessKeyboardInput(BACKWARD, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_A)) {
        cam.ProcessKeyboardInput(LEFT, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_D)) {
        cam.ProcessKeyboardInput(RIGHT, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_Q)) {
        cam.ProcessKeyboardInput(DOWN, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_E)) {
        cam.ProcessKeyboardInput(UP, dt);
      }

      // process input for panning
      ImVec2 const currMousePos{ ImGui::GetMousePos() };
      ImVec2 const mouseDelta{ currMousePos - previousMousePos };
      cam.ProcessMouseInput(mouseDelta.x / windowSize.x, mouseDelta.y / windowSize.y);
      previousMousePos = currMousePos;
    }
    else {
      isDragging = false;

      if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        if (!isPanning) {
          previousMousePos = ImGui::GetMousePos();
          isPanning = true;
        }
        ImGui::SetMouseCursor(ImGuiMouseCursor_None); // hide cursor when panning

        ImVec2 const currMousePos{ ImGui::GetMousePos() };
        ImVec2 const mouseDelta{ currMousePos - previousMousePos };
        cam.MoveAlongPlane(mouseDelta.x / windowSize.x, mouseDelta.y / windowSize.y);
        previousMousePos = currMousePos;
      }
      else {
        isPanning = false;
      }
    }

    float const scrollDelta{ ImGui::GetIO().MouseWheel };
    if (glm::abs(scrollDelta) > glm::epsilon<float>()) {
      cam.ProcessMouseScroll(scrollDelta);
    }
  }

  void Viewport::ReceivePayload()
  {
    if (ImGui::BeginDragDropTarget())
    {
      ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
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
        case AssetPayload::MODEL:
        {
          // @TODO: ABSTRACT MORE; MAKE IT EASIER TO ADD A MESH
          ECS::Entity newEntity{ ECS::EntityManager::GetInstance().CreateEntityWithTag(assetPayload.GetFileName()) };
          auto meshSrc{ std::make_shared<Graphics::Mesh>(Graphics::MeshFactory::CreateModelFromImport(assetPayload.GetFilePath())) };
          newEntity.EmplaceComponent<Component::Mesh>(meshSrc, assetPayload.GetFileName());
          break;
        }
        case AssetPayload::SPRITE:
        case AssetPayload::AUDIO:
        default:
          break;
        }
      }
      ImGui::EndDragDropTarget();
    }
  }

} // namespace GUI

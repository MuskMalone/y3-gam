/*!*********************************************************************
\file   Viewport.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the editor viewport
        window of the editor. Renders the scene onto the ImGui window
        by attaching the framebuffer as an image. Also allows input
        operations such as DragDrop and selection / gizmos (in future).

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Viewport.h"
#include <imgui/imgui.h>
#include <GUI/Helpers/AssetPayload.h>
#include "AssetBrowser.h"
#include <Events/EventManager.h>
#include <FrameRateController/FrameRateController.h>
#include <Core/Entity.h>
#include <Core/Components/Mesh.h>
#include <Core/Components/Transform.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Core/EntityManager.h>
#include <GUI/GUIManager.h>
#include <Asset/IGEAssets.h>

namespace GUI
{
  // for panning camera to entity when double-clicked upon
  static bool sMovingToEntity{ false };
  static glm::vec3 sTargetPosition{};

  Viewport::Viewport(const char* name, Graphics::EditorCamera& camera) : GUIWindow(name),
    mEditorCam{ camera }, mIsPanning{ false }, mIsDragging{ false } {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ENTITY_ZOOM, &Viewport::HandleEvent, this);
  }

  void Viewport::Render(std::shared_ptr<Graphics::Framebuffer> const& framebuffer)
  {
    ImGui::Begin(mWindowName.c_str());

    ImVec2 const vpSize = ImGui::GetContentRegionAvail();
    ImVec2 const vpStartPos{ ImGui::GetCursorScreenPos() };

    // only register input if viewport is focused
    bool const checkInput{ mIsDragging || mIsPanning || sMovingToEntity };
    if ((ImGui::IsWindowFocused() && ImGui::IsWindowHovered()) || checkInput) {
      ProcessCameraInputs();
    }
    // auto focus window when middle or right-clicked upon
    else if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle))) {
      ImGui::FocusWindow(ImGui::GetCurrentWindow());
    }

    // update framebuffer
    ImGui::Image(
      reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(framebuffer->GetColorAttachmentID())),
      vpSize,
      ImVec2(0, 1),
      ImVec2(1, 0)
    );

    ReceivePayload();

    // object picking
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      ImVec2 const offset{ ImGui::GetMousePos() - vpStartPos };

      // check if clicking outside viewport
      if (!(offset.x < 0 || offset.x > vpSize.x || offset.y < 0 || offset.y > vpSize.y)) {
        Graphics::FramebufferSpec const& fbSpec{ framebuffer->GetFramebufferSpec() };

        framebuffer->Bind();
        int const entityId{ framebuffer->ReadPixel(1,
          static_cast<int>(offset.x / vpSize.x * static_cast<float>(fbSpec.width)),
          static_cast<int>((vpSize.y - offset.y) / vpSize.y * static_cast<float>(fbSpec.height))) };
        framebuffer->Unbind();

        if (entityId > 0) {
          GUIManager::SetSelectedEntity(static_cast<ECS::Entity::EntityID>(entityId));
        }
      }
    }

    if (GUIManager::GetSelectedEntity() && 
        GUIManager::GetSelectedEntity().HasComponent<Component::Transform>()) {
        ImGuizmo::SetDrawlist();
        ImVec2 windowPos{ ImGui::GetWindowPos() };

        float windowWidth { ImGui::GetWindowWidth() };
        float windowHeight{ ImGui::GetWindowHeight() };
        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowWidth, windowHeight);
        auto& transform{ GUIManager::GetSelectedEntity().GetComponent<Component::Transform>() };
        auto modelMatrix{ transform.worldMtx };
        auto modelMatrixPrev{ transform.worldMtx };
        auto viewMatrix{ mEditorCam.GetViewMatrix() };
        auto projMatrix{ mEditorCam.GetProjMatrix() };

        static auto currentOperation = ImGuizmo::TRANSLATE ;
        if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered()) {
            if (ImGui::IsKeyPressed(ImGuiKey_T))
                currentOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) 
                currentOperation = ImGuizmo::ROTATE;
            else if (ImGui::IsKeyPressed(ImGuiKey_S)) 
                currentOperation = ImGuizmo::SCALE;
        }
        ImGuizmo::Manipulate(
            glm::value_ptr(viewMatrix),           
            glm::value_ptr(projMatrix),     
            currentOperation,                           
            ImGuizmo::LOCAL,                 
            glm::value_ptr(modelMatrix)          
        );
        if (ImGuizmo::IsUsing()) {
            glm::vec3 s{}, r{}, t{};
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix),
                glm::value_ptr(t), glm::value_ptr(r), glm::value_ptr(s));
            glm::vec3 s2{}, r2{}, t2{};
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrixPrev),
                glm::value_ptr(t2), glm::value_ptr(r2), glm::value_ptr(s2));
            if (currentOperation == ImGuizmo::TRANSLATE) {
                transform.position += std::move(t - t2);

            }
            if (currentOperation == ImGuizmo::ROTATE) {
                auto localRot{ transform.eulerAngles + std::move(r - r2) };
                transform.SetLocalRotWithEuler(localRot);
            }
            if (currentOperation == ImGuizmo::SCALE) {
                transform.scale += std::move(s - s2);
            }
            transform.modified = true;
            TransformHelpers::UpdateWorldTransform(GUIManager::GetSelectedEntity());  // must call this to update world transform according to changes to local
        }
    }
    ImGui::End();
  }

  void Viewport::ProcessCameraInputs() {
    static ImVec2 previousMousePos;
    using enum Graphics::EditorCamera::CameraMovement;  // C++20 <3
    float const dt{ Performance::FrameRateController::GetInstance().GetDeltaTime() };

    ImVec2 const windowSize{ ImGui::GetContentRegionAvail() };

    // only allow movement and panning if right-click held
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
      sMovingToEntity = false;
      if (!mIsPanning && !mIsDragging) {
        previousMousePos = ImGui::GetMousePos();
        mIsDragging = true;
      }
      ImGui::SetMouseCursor(ImGuiMouseCursor_None); // hide cursor when looking

      // process input for movement
      if (ImGui::IsKeyDown(ImGuiKey_W)) {
        mEditorCam.ProcessKeyboardInput(FORWARD, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_S)) {
        mEditorCam.ProcessKeyboardInput(BACKWARD, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_A)) {
        mEditorCam.ProcessKeyboardInput(LEFT, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_D)) {
        mEditorCam.ProcessKeyboardInput(RIGHT, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_Q)) {
        mEditorCam.ProcessKeyboardInput(DOWN, dt);
      }
      if (ImGui::IsKeyDown(ImGuiKey_E)) {
        mEditorCam.ProcessKeyboardInput(UP, dt);
      }

      // process input for panning
      ImVec2 const currMousePos{ ImGui::GetMousePos() };
      ImVec2 const mouseDelta{ currMousePos - previousMousePos };
      mEditorCam.ProcessMouseInput(mouseDelta.x / windowSize.x, mouseDelta.y / windowSize.y);
      previousMousePos = currMousePos;
    }
    else {
      mIsDragging = false;

      if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        if (!mIsPanning) {
          previousMousePos = ImGui::GetMousePos();
          mIsPanning = true;
        }
        ImGui::SetMouseCursor(ImGuiMouseCursor_None); // hide cursor when panning

        ImVec2 const currMousePos{ ImGui::GetMousePos() };
        ImVec2 const mouseDelta{ currMousePos - previousMousePos };
        mEditorCam.MoveAlongPlane(mouseDelta.x / windowSize.x, mouseDelta.y / windowSize.y);
        previousMousePos = currMousePos;
      }
      else {
        mIsPanning = false;
      }
    }
    
    ReceivePayload();

    if (sMovingToEntity) {
      glm::vec3 const endPos{ sTargetPosition - mEditorCam.GetForwardVector() * 10.f };
      if (glm::distance2(endPos, mEditorCam.GetPosition()) > 0.5f) {
        mEditorCam.MoveTowardsPoint(endPos, 2.f * dt);
      }
      else {
        mEditorCam.SetPosition(endPos);
        sMovingToEntity = false;
      }
    }

    float const scrollDelta{ ImGui::GetIO().MouseWheel };
    if (glm::abs(scrollDelta) > glm::epsilon<float>()) {
      mEditorCam.ProcessMouseScroll(scrollDelta);
    }
  }

  EVENT_CALLBACK_DEF(Viewport, HandleEvent) {
    sTargetPosition = CAST_TO_EVENT(Events::ZoomInOnEntity)->mEntity.GetComponent<Component::Transform>().worldPos;
    sMovingToEntity = true;
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
          auto meshSrc{ std::make_shared<Graphics::Mesh>(IGE_ASSETMGR.LoadRef<IGE::Assets::MeshAsset>(assetPayload.GetFilePath())) };
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

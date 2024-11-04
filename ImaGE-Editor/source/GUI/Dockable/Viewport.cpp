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
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Core/EntityManager.h>
#include <GUI/GUIManager.h>
#include <Asset/IGEAssets.h>

namespace {
  /*!*********************************************************************
    \brief
      Projects a 3d vector onto the camera's view plane
    \param vector
      The vector
    \param cam
      The camera
    \return
      The projected 2d vector on the plane
    ************************************************************************/
  glm::vec2 ProjVectorOnCamPlane(glm::vec3 const& vector , Graphics::EditorCamera const& cam);

  /*!*********************************************************************
  \brief
    Returns the root parent entity of the given entity
  \param entity
    The entity
  \return
    The root entity
  ************************************************************************/
  ECS::Entity GetRootEntity(ECS::Entity entity) {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
    if (!em.HasParent(entity)) { return entity; }

    return GetRootEntity(em.GetParentEntity(entity));
  }
}

namespace GUI
{
  // for panning camera to entity when double-clicked upon
  static bool sMovingToEntity{ false };
  static glm::vec3 sTargetPosition, sMoveDir;
  static float sDistToCover;
  static ECS::Entity sPrevSelectedEntity;

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

      if (!UpdateGuizmos()) {
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
                      ECS::Entity const selected{ static_cast<ECS::Entity::EntityID>(entityId) },
                          root{ GetRootEntity(selected) };
                      sPrevSelectedEntity = root == sPrevSelectedEntity ? selected : root;
                      GUIManager::SetSelectedEntity(sPrevSelectedEntity);
                  }
                  else {
                      sPrevSelectedEntity = {};
                      GUIManager::SetSelectedEntity({});
                  }
              }
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

    // move camera towards entity if the event has been triggered
    if (sMovingToEntity) {
      if (glm::distance2(sTargetPosition, mEditorCam.GetPosition()) > 0.5f) {
        float const movePercentageThisFrame{ Performance::FrameRateController::GetInstance().GetDeltaTime() / 0.5f * sDistToCover };
        glm::vec3 const remainingDist{ sTargetPosition - mEditorCam.GetPosition() };
        glm::vec3 offsetThisFrame{ sMoveDir * movePercentageThisFrame };

        // clamp the distance so we don't overshoot
        float const squaredDist{ glm::length2(offsetThisFrame) };
        if (squaredDist > glm::length2(remainingDist)) {
          offsetThisFrame = remainingDist;
        }

        mEditorCam.MoveCamera(offsetThisFrame);
      }
      else {
        mEditorCam.SetPosition(sTargetPosition);
        sMovingToEntity = false;
      }
    }

    float const scrollDelta{ ImGui::GetIO().MouseWheel };
    if (glm::abs(scrollDelta) > glm::epsilon<float>()) {
      mEditorCam.ProcessMouseScroll(scrollDelta);
    }
  }

  EVENT_CALLBACK_DEF(Viewport, HandleEvent) {
    Component::Transform const& trans{ CAST_TO_EVENT(Events::ZoomInOnEntity)->mEntity.GetComponent<Component::Transform>() };
    // project the entity's scale onto the camera's view plane
    glm::vec2 const projectedEntityScale{ ProjVectorOnCamPlane(trans.worldScale, mEditorCam) };
    
    // then offset backwards from the entity's position based on the larger scale component and scale factor
    sTargetPosition = trans.worldPos - mEditorCam.GetForwardVector()
      * glm::max(projectedEntityScale.x, projectedEntityScale.y) * sEntityScaleFactor;
    glm::vec3 const totalDist{ sTargetPosition - mEditorCam.GetPosition() };
    sMoveDir = glm::normalize(totalDist);
    sDistToCover = glm::length(totalDist);
    sMovingToEntity = true;
  }

  bool Viewport::UpdateGuizmos() const {
    ECS::Entity selectedEntity{ GUIManager::GetSelectedEntity() };
    if (!selectedEntity || !selectedEntity.HasComponent<Component::Transform>()) { return false; }

    bool usingGuizmos{ false };
    ImGuizmo::SetDrawlist();
    ImVec2 windowPos{ ImGui::GetWindowPos() };

    float windowWidth{ ImGui::GetWindowWidth() };
    float windowHeight{ ImGui::GetWindowHeight() };
    ImGuizmo::SetRect(windowPos.x, windowPos.y, windowWidth, windowHeight);
    auto& transform{ selectedEntity.GetComponent<Component::Transform>() };
    auto modelMatrix{ transform.worldMtx };
    auto modelMatrixPrev{ transform.worldMtx };
    auto viewMatrix{ mEditorCam.GetViewMatrix() };
    auto projMatrix{ mEditorCam.GetProjMatrix() };

    static auto currentOperation = ImGuizmo::TRANSLATE;
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
      usingGuizmos = true;
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
      TransformHelpers::UpdateWorldTransform(selectedEntity);  // must call this to update world transform according to changes to local
    }

    return usingGuizmos;
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
          ECS::Entity newEntity{ ECS::EntityManager::GetInstance().CreateEntityWithTag(assetPayload.GetFileName()) };
          IGE::Assets::GUID const& meshSrc{ IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>(assetPayload.GetFilePath()) };
          newEntity.EmplaceComponent<Component::Mesh>(meshSrc, assetPayload.GetFileName(), true);
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

namespace {
  glm::vec2 ProjVectorOnCamPlane(glm::vec3 const& vector, Graphics::EditorCamera const& cam) {
    // projection = vector - dot(vector, normal) * normal
    glm::vec3 const camFwdVec{ cam.GetForwardVector() };
    glm::vec3 const projectedVec{ vector - glm::dot(vector, camFwdVec) * camFwdVec };

    return { glm::dot(cam.GetRightVector(), projectedVec), glm::dot(cam.GetUpVector(), projectedVec) };
  }
}

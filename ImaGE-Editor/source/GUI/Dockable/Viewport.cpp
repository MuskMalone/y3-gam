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

#include "Graphics/Renderer.h"
#include <Events/EventManager.h>
#include <FrameRateController/FrameRateController.h>
#include <Scenes/SceneManager.h>
#include <Core/EntityManager.h>
#include <Commands/CommandManager.h>

#include <GUI/Helpers/AssetPayload.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Core/Entity.h>
#include <Core/Components/Mesh.h>
#include <Core/Components/Transform.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Core/Components/PrefabOverrides.h>
#include <Core/Components/Light.h>
#include <GUI/GUIVault.h>
#include <Asset/IGEAssets.h>
#include <Graphics/RenderPass/GeomPass.h>
#include <EditorEvents.h>

namespace {
  // for panning camera to entity when double-clicked upon
  static bool sMovingToEntity{ false };
  static glm::vec3 sTargetPosition, sMoveDir;
  static float sDistToCover;
  static ECS::Entity sPrevSelectedEntity{}, sPrevSelectedLeaf{};
  static std::unique_ptr<GUI::AssetPayload> sDroppedPayload{};

  // for multi-select
  static bool sCtrlHeld{ false };

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
  glm::vec2 ProjVectorOnCamPlane(glm::vec3 const& vector, std::shared_ptr<Graphics::EditorCamera> const& cam);

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

  ECS::Entity ConstructEntity(IGE::Assets::GUID const& guid, Graphics::MeshSource const& meshSource);
}

namespace GUI
{
  Viewport::Viewport(const char* name, std::shared_ptr<Graphics::EditorCamera> camera) : GUIWindow(name),
    mEditorCam{ std::move(camera) }, mIsPanning{ false }, mRightClickHeld{ false }, mFocusWindow{ false } {
    SUBSCRIBE_CLASS_FUNC(Events::ZoomInOnEntity, &Viewport::OnEntityDoubleClicked, this);
    SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &Viewport::OnSceneStart, this);
    SUBSCRIBE_CLASS_FUNC(Events::CollectEditorSceneData, &Viewport::OnCollectEditorData, this);
    SUBSCRIBE_CLASS_FUNC(Events::LoadEditorSceneData, &Viewport::OnLoadEditorData, this);
  }

  void Viewport::Render(std::shared_ptr<Graphics::Framebuffer> const& framebuffer)
  {
      ImGui::Begin(mWindowName.c_str());

      ImVec2 const vpSize = ImGui::GetContentRegionAvail();
      ImVec2 const vpStartPos{ ImGui::GetCursorScreenPos() };

      sCtrlHeld = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

      // only register input if viewport is focused
      bool const checkInput{ (ImGui::IsWindowFocused() && ImGui::IsWindowHovered()) || mRightClickHeld || mIsPanning || sMovingToEntity };
      if (checkInput) {
        // check for delete key
        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && GUIVault::GetSelectedEntity()) {
          ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
          auto entities{ GUIVault::GetSelectedEntities() };
          if (!entities.empty()) {
            for (ECS::Entity e : entities) {
              em.RemoveEntity(e);
            }
            GUIVault::ClearSelectedEntities();
          }
          else {
            em.RemoveEntity(GUIVault::GetSelectedEntity());
          }

          GUIVault::SetSelectedEntity(ECS::Entity());
          IGE_EVENTMGR.DispatchImmediateEvent<Events::SceneModifiedEvent>();
        }

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

      // set focus to game vp if scene stopped
      if (mFocusWindow) {
        ImGui::SetWindowFocus();
        mFocusWindow = false;
      }

      float const windowRight{ vpStartPos.x + vpSize.x };
      ImVec2 const topLeft{ windowRight - 128.f , vpStartPos.y }, size{ 128.f, 128.f };

      if (UpdateViewManipulate(topLeft, size)) {
        mEditorCam->UpdateFromViewMtx(mEditorCam->viewMatrix);
      }
      else {
        bool const viewManipulateWindowClicked{ ImGui::IsMouseClicked(ImGuiMouseButton_Left)
        && ImGui::IsMouseHoveringRect(topLeft, { windowRight, vpStartPos.y + 128.f }) };

        if (!viewManipulateWindowClicked && !UpdateGuizmos() && checkInput) {
          // object picking
          if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 const offset{ ImGui::GetMousePos() - vpStartPos };

            // check if clicking outside viewport
            if (!(offset.x < 0 || offset.x > vpSize.x || offset.y < 0 || offset.y > vpSize.y)) {

              auto const& geomPass{ Graphics::Renderer::GetPass<Graphics::GeomPass>() };
              auto const& pickFb{ geomPass->GetTargetFramebuffer() };
              Graphics::FramebufferSpec const& fbSpec{ pickFb->GetFramebufferSpec() };

              pickFb->Bind();
              int const entityId{ pickFb->ReadPixel(1,
                static_cast<int>(offset.x / vpSize.x * static_cast<float>(fbSpec.width)),
                static_cast<int>((vpSize.y - offset.y) / vpSize.y * static_cast<float>(fbSpec.height))) };
              pickFb->Unbind();

              if (entityId >= 0) {
                ECS::Entity const selected{ static_cast<ECS::Entity::EntityID>(entityId) },
                  root{ GetRootEntity(selected) };
                ECS::EntityManager& em{ IGE_ENTITYMGR };

                if (selected == sPrevSelectedLeaf && em.HasParent(sPrevSelectedEntity)) {
                  sPrevSelectedEntity = em.GetParentEntity(sPrevSelectedEntity);
                }
                else {
                  sPrevSelectedLeaf = sPrevSelectedEntity = selected;
                }

                if (sCtrlHeld) {
                  ECS::Entity const curr{ GUIVault::GetSelectedEntity() };
                  if (GUIVault::GetSelectedEntities().empty() && curr) {
                    GUIVault::AddSelectedEntity(curr);
                  }

                  if (GUIVault::IsEntitySelected(root)) {
                    GUIVault::RemoveSelectedEntity(root);
                    if (GUIVault::GetSelectedEntities().empty()) {
                      GUIVault::SetSelectedEntity({});
                    }
                    else {
                      GUIVault::SetSelectedEntity(*GUIVault::GetSelectedEntities().begin());
                    }
                  }
                  else {
                    GUIVault::AddSelectedEntity(root);
                    GUIVault::SetSelectedEntity(sPrevSelectedEntity);
                  }
                } // end if ctrlHeld
                else {
                  GUIVault::ClearSelectedEntities();
                  GUIVault::SetSelectedEntity(sPrevSelectedEntity);
                }

                QUEUE_EVENT(Events::EntityScreenPicked, sPrevSelectedEntity);
              } // end if entityId >= 0
              else {
                sPrevSelectedEntity = {};
                GUIVault::SetSelectedEntity({});
                GUIVault::ClearSelectedEntities();
              }
            } // if within bounds
          } // if left click
        }
      } // else block UpdateViewManipulate


      if (mEditorCam->modified) {
        mEditorCam->UpdateMatrices();
        mEditorCam->modified = false;
      }
      
      ReceivePayload();

      UnsavedChangesPopup();

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
      if (!mIsPanning && !mRightClickHeld) {
        previousMousePos = ImGui::GetMousePos();
        mRightClickHeld = true;
      }
      ImGui::SetMouseCursor(ImGuiMouseCursor_None); // hide cursor when looking

      bool const shiftHeld{ ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift) };
      if (shiftHeld) {
        mEditorCam->mMoveSpeed = 2.f;
      }

      bool moved{ false };
      // process input for movement
      if (ImGui::IsKeyDown(ImGuiKey_W)) {
        mEditorCam->ProcessKeyboardInput(FORWARD, dt);
        moved = true;
      }
      if (ImGui::IsKeyDown(ImGuiKey_S)) {
        mEditorCam->ProcessKeyboardInput(BACKWARD, dt);
        moved = true;
      }
      if (ImGui::IsKeyDown(ImGuiKey_A)) {
        mEditorCam->ProcessKeyboardInput(LEFT, dt);
        moved = true;
      }
      if (ImGui::IsKeyDown(ImGuiKey_D)) {
        mEditorCam->ProcessKeyboardInput(RIGHT, dt);
        moved = true;
      }
      if (ImGui::IsKeyDown(ImGuiKey_Q)) {
        mEditorCam->ProcessKeyboardInput(DOWN, dt);
        moved = true;
      }
      if (ImGui::IsKeyDown(ImGuiKey_E)) {
        mEditorCam->ProcessKeyboardInput(UP, dt);
        moved = true;
      }
      
      if (!shiftHeld) {
        if (moved) {
          mEditorCam->mMoveSpeed *= 1.02f;
        }
        else {
          mEditorCam->mMoveSpeed = 15.f;
        }
      }

      // process input for panning
      ImVec2 const currMousePos{ ImGui::GetMousePos() };
      ImVec2 const mouseDelta{ currMousePos - previousMousePos };
      mEditorCam->ProcessMouseInput(mouseDelta.x / windowSize.x, mouseDelta.y / windowSize.y);
      previousMousePos = currMousePos;
    }
    else {
      mRightClickHeld = false;
      mEditorCam->mMoveSpeed = 15.f;

      if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        if (!mIsPanning) {
          previousMousePos = ImGui::GetMousePos();
          mIsPanning = true;
        }
        ImGui::SetMouseCursor(ImGuiMouseCursor_None); // hide cursor when panning

        ImVec2 const currMousePos{ ImGui::GetMousePos() };
        ImVec2 const mouseDelta{ currMousePos - previousMousePos };
        mEditorCam->MoveAlongPlane(mouseDelta.x / windowSize.x, mouseDelta.y / windowSize.y);
        previousMousePos = currMousePos;
      }
      else {
        mIsPanning = false;
      }
    }
    
    ReceivePayload();

    // move camera towards entity if the event has been triggered
    if (sMovingToEntity) {
      if (glm::distance2(sTargetPosition, mEditorCam->position) > 0.5f) {
        float const movePercentageThisFrame{ Performance::FrameRateController::GetInstance().GetDeltaTime() / 0.5f * sDistToCover };
        glm::vec3 const remainingDist{ sTargetPosition - mEditorCam->position };
        glm::vec3 offsetThisFrame{ sMoveDir * movePercentageThisFrame };

        // clamp the distance so we don't overshoot
        float const squaredDist{ glm::length2(offsetThisFrame) };
        if (squaredDist > glm::length2(remainingDist)) {
          offsetThisFrame = remainingDist;
        }

        mEditorCam->MoveCamera(offsetThisFrame);
      }
      else {
        mEditorCam->position = sTargetPosition;
        sMovingToEntity = false;
      }
      mEditorCam->Modified();
    }

    float const scrollDelta{ ImGui::GetIO().MouseWheel };
    if (glm::abs(scrollDelta) > glm::epsilon<float>()) {
      mEditorCam->ProcessMouseScroll(scrollDelta);
    }
  }

  EVENT_CALLBACK_DEF(Viewport, OnEntityDoubleClicked) {
    ECS::Entity const entity{ CAST_TO_EVENT(Events::ZoomInOnEntity)->mEntity };
    Component::Transform const& trans{ entity.GetComponent<Component::Transform>() };
    glm::vec3 scale{ trans.worldScale };

    // if entity has mesh, apply its aabb on top of scale
    if (entity.HasComponent<Component::Mesh>()) {
      IGE::Assets::GUID const guid{ entity.GetComponent<Component::Mesh>().meshSource };
      if (guid) {
        scale *= 2.f * IGE_ASSETMGR.GetAsset<IGE::Assets::ModelAsset>(guid)->mMeshSource.GetBoundingBox().halfExtents;
      }
    }

    // project the entity's scale onto the camera's view plane
    glm::vec2 const projectedEntityScale{ ProjVectorOnCamPlane(scale, mEditorCam) };
    
    // then offset backwards from the entity's position based on the larger scale component and scale factor
    sTargetPosition = trans.worldPos - mEditorCam->GetForwardVector()
      * glm::max(projectedEntityScale.x, projectedEntityScale.y) * sEntityScaleFactor;
    glm::vec3 const totalDist{ sTargetPosition - mEditorCam->position };
    sMoveDir = glm::normalize(totalDist);
    sDistToCover = glm::length(totalDist);
    sMovingToEntity = true;
  }

  EVENT_CALLBACK_DEF(Viewport, OnSceneStart) {
    // set flag if scene stopped
    if (CAST_TO_EVENT(Events::SceneStateChange)->mNewState == Events::SceneStateChange::STOPPED) {
      mFocusWindow = true;
    }
  }

  bool Viewport::UpdateViewManipulate(ImVec2 const& windowPos, ImVec2 const& size) {
    ImGuizmo::ViewManipulate(glm::value_ptr(mEditorCam->viewMatrix), 8.f, windowPos, size, 0x10101010);

    return ImGuizmo::IsUsingViewManipulate();
  }

  bool Viewport::UpdateGuizmos() const {
    ECS::Entity selectedEntity{ GUIVault::GetSelectedEntity() };
    if (!selectedEntity || !selectedEntity.HasComponent<Component::Transform>()) { return false; }
    static Component::PrefabOverrides* prefabOverrides{ nullptr };
    prefabOverrides = selectedEntity.HasComponent<Component::PrefabOverrides>() ? 
      &selectedEntity.GetComponent<Component::PrefabOverrides>() : nullptr;

    bool usingGuizmos{ false };
    ImGuizmo::SetDrawlist();
    ImVec2 windowPos{ ImGui::GetWindowPos() };

    float const windowWidth{ ImGui::GetWindowWidth() };
    float const windowHeight{ ImGui::GetWindowHeight() };
    ImGuizmo::SetRect(windowPos.x, windowPos.y, windowWidth, windowHeight);
    Component::Transform& transform{ selectedEntity.GetComponent<Component::Transform>() },
      oldTrans{ transform };  // old cpy for undo/redo

    glm::mat4 const& viewMatrix{ mEditorCam->viewMatrix };
    glm::mat4 const projMatrix{ mEditorCam->GetProjMatrix() };
    glm::mat4 worldMtx{ transform.worldMtx };

    static auto currentOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered()) {
      if (!mRightClickHeld) {
      if (ImGui::IsKeyPressed(ImGuiKey_T))
        currentOperation = ImGuizmo::TRANSLATE;
      if (ImGui::IsKeyPressed(ImGuiKey_R))
        currentOperation = ImGuizmo::ROTATE;
      else if (ImGui::IsKeyPressed(ImGuiKey_S))
        currentOperation = ImGuizmo::SCALE;
      }
    }

    ImGuizmo::Manipulate(
      glm::value_ptr(viewMatrix),
      glm::value_ptr(projMatrix),
      currentOperation,
      ImGuizmo::WORLD,
      glm::value_ptr(worldMtx)
    );

    // since ImGuizmo uses the worldMtx, we'll need to translate the
    // changes to the entity's local transform and set the "modified" flag to true.
    // The transform system will handle the rest
    if (ImGuizmo::IsUsing()) {
      usingGuizmos = true;
      glm::vec3 s{}, r{}, t{};
      ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(worldMtx),
        glm::value_ptr(t), glm::value_ptr(r), glm::value_ptr(s));

      // for each operation, if multi-select active, update all their transforms
      bool const multiSelectActive{ !GUIVault::GetSelectedEntities().empty() };
      if (currentOperation == ImGuizmo::TRANSLATE) {
        glm::vec3 const offset{ t - transform.worldPos };
        if (multiSelectActive) {
          for (ECS::Entity e : GUIVault::GetSelectedEntities()) {
            e.GetComponent<Component::Transform>().worldPos += offset;
          }
        }
        else {
          transform.worldPos += offset;
        }
      }
      else if (currentOperation == ImGuizmo::ROTATE) {
       // glm::quat const offset{ glm::quat(glm::radians(r)) * glm::inverse(transform.worldRot) };
        //glm::vec3 const eulerOffset{ r - transform.eulerAngles };

        if (multiSelectActive) {
          for (ECS::Entity e : GUIVault::GetSelectedEntities()) {
            Component::Transform& trans{ e.GetComponent<Component::Transform>() };
            trans.worldRot = glm::quat(glm::radians(r));
            //trans.rotation = offset * trans.rotation;
            //trans.eulerAngles += eulerOffset;
          }
        }
        else {
          //transform.rotation = offset * transform.rotation;
          //transform.eulerAngles += eulerOffset;
          transform.worldRot = glm::quat(glm::radians(r));
        }
      }
      else if (currentOperation == ImGuizmo::SCALE) {
        glm::vec3 const offset{ s - transform.worldScale };

        if (multiSelectActive) {
          for (ECS::Entity e : GUIVault::GetSelectedEntities()) {
            e.GetComponent<Component::Transform>().worldScale += offset;
          }
        }
        else {
          transform.worldScale += offset;
        }
      }

      // update all the PrefabOverrides if needed
      if (multiSelectActive) {
        ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };

        for (ECS::Entity e : GUIVault::GetSelectedEntities()) {
          e.GetComponent<Component::Transform>().modified = true;
          TransformHelpers::UpdateLocalTransform(e);  // TEMP FIX

          if (!e.HasComponent<Component::PrefabOverrides>()) { continue; }

          Component::PrefabOverrides& pfbOverrides{ e.GetComponent<Component::PrefabOverrides>() };

          if (pfbOverrides.IsComponentModified<Component::Transform>() || pfbOverrides.subDataId != Prefabs::PrefabSubData::BasePrefabId) {
            pfbOverrides.AddComponentOverride<Component::Transform>();
          }
          else if (currentOperation != ImGuizmo::TRANSLATE) {
            pfbOverrides.AddComponentOverride<Component::Transform>();
          }
          if (selectedEntity.HasComponent<Component::Light>()) {
            Component::Light& lightComp{ selectedEntity.GetComponent<Component::Light>() };
            if (lightComp.type == Component::DIRECTIONAL && lightComp.castShadows) {
              lightComp.shadowConfig.shadowModified = true;
            }
          }
        }
      }
      else {
        // if single select, simply update that entity normally
        transform.modified = true;
        TransformHelpers::UpdateLocalTransform(selectedEntity); // TEMP FIX
        if (prefabOverrides) {
          if (prefabOverrides->IsComponentModified<Component::Transform>() || prefabOverrides->subDataId != Prefabs::PrefabSubData::BasePrefabId) {
            prefabOverrides->AddComponentOverride<Component::Transform>();
          }
          else if (currentOperation != ImGuizmo::TRANSLATE) {
            prefabOverrides->AddComponentOverride<Component::Transform>();
          }
        }

        if (selectedEntity.HasComponent<Component::Light>()) {
          Component::Light& lightComp{ selectedEntity.GetComponent<Component::Light>() };
          if (lightComp.type == Component::DIRECTIONAL && lightComp.castShadows) {
            lightComp.shadowConfig.shadowModified = true;
          }
        }
      }

      IGE_CMDMGR.AddCommand("Transform", selectedEntity, std::move(oldTrans));
      QUEUE_EVENT(Events::SceneModifiedEvent);
    }

    return usingGuizmos;
  }

  void Viewport::ReceivePayload()
  {
    bool const noScene{ IGE_SCENEMGR.NoSceneSelected() };
    if (ImGui::BeginDragDropTarget())
    {
      ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
      if (drop)
      {
        AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
        switch (assetPayload.mAssetType)
        {
        case AssetPayload::SCENE:
        {
          // if scene has unsaved changes, trigger popup
          if (GUIVault::IsSceneModified()) {
            // temporarily save the current payload
            sDroppedPayload = std::make_unique<GUI::AssetPayload>(std::move(assetPayload));
            ImGui::OpenPopup(sUnsavedChangesPopupTitle);
          }
          else {
            QUEUE_EVENT(Events::LoadSceneEvent, assetPayload.GetFileName(), assetPayload.GetFilePath());
          }
          break;
        }
        case AssetPayload::PREFAB:
        {
          if (noScene) {
            ImGui::EndDragDropTarget();
            return;
          }

          // @TODO: Convert screen to world pos when viewport is up
          try {
            IGE::Assets::AssetManager& assetMan{ IGE_ASSETMGR };
            IGE::Assets::GUID guid{ assetMan.LoadRef<IGE::Assets::PrefabAsset>(assetPayload.GetFilePath()) };

            ECS::Entity newEntity{ assetMan.GetAsset<IGE::Assets::PrefabAsset>(guid)->mPrefabData.Construct(guid, {}) };
            GUIVault::SetSelectedEntity(newEntity);
          }
          catch (Debug::ExceptionBase&) {
            IGE_DBGLOGGER.LogError("Unable to get GUID of " + assetPayload.GetFilePath());
          }
          break;
        }
        case AssetPayload::MODEL:
        {
          if (noScene) {
            ImGui::EndDragDropTarget();
            return;
          }

          try {
            IGE::Assets::GUID const& meshSrc{ IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>(assetPayload.GetFilePath()) };
            ECS::Entity const newEntity{ IGE_ASSETMGR.GetAsset<IGE::Assets::ModelAsset>(meshSrc)->mMeshSource.ConstructEntity(meshSrc, assetPayload.GetFileName()) };
            GUIVault::SetSelectedEntity(newEntity);
          }
          catch (Debug::ExceptionBase&) {
            IGE_DBGLOGGER.LogError("Unable to get GUID of " + assetPayload.GetFilePath());
          }
          break;
        }
        default:
          break;
        }
      }
      ImGui::EndDragDropTarget();
    }
  }

  void Viewport::UnsavedChangesPopup() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(sUnsavedChangesPopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) { return; }
    
    bool close{ false };
    {
      ImGui::Text("Would you like to save your changes to ");
      ImGui::SameLine();
      float const denom{ 1 / 255.f };
      ImGui::TextColored(ImVec4(167.f * denom, 90 * denom, 35 * denom, 255), (IGE_SCENEMGR.GetSceneName() + ".scn").c_str());
      ImGui::SameLine();
      ImGui::Text("?");
    }

    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.5f - ImGui::CalcTextSize("Save Discard Ch").x);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.55f, 0.f, 1.f));
    if (ImGui::Button("Save"))
    {
      IGE_EVENTMGR.DispatchImmediateEvent<Events::SaveSceneEvent>(GUIVault::sSerializePrettyScene);
      QUEUE_EVENT(Events::LoadSceneEvent, sDroppedPayload->GetFileName(), sDroppedPayload->GetFilePath());
      close = true;
    }
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.2f, 0.2f, 1.f));
    ImGui::SameLine();
    
    if (ImGui::Button("Discard Changes")) {
      QUEUE_EVENT(Events::LoadSceneEvent, sDroppedPayload->GetFileName(), sDroppedPayload->GetFilePath());
      close = true;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      close = true;
    }

    if (close) {
      sDroppedPayload.reset();
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  EVENT_CALLBACK_DEF(Viewport, OnCollectEditorData) {
    GUI::SceneEditorConfig& cfg{ CAST_TO_EVENT(Events::CollectEditorSceneData)->mSceneConfig };
    cfg.editorCam = *mEditorCam;
  }

  EVENT_CALLBACK_DEF(Viewport, OnLoadEditorData) {
    *mEditorCam = CAST_TO_EVENT(Events::CollectEditorSceneData)->mSceneConfig.editorCam;
    mEditorCam->UpdateMatrices();
  }

} // namespace GUI

namespace {
  glm::vec2 ProjVectorOnCamPlane(glm::vec3 const& vector, std::shared_ptr<Graphics::EditorCamera> const& cam) {
    // projection = vector - dot(vector, normal) * normal
    glm::vec3 const camFwdVec{ cam->GetForwardVector() };
    glm::vec3 const projectedVec{ vector - glm::dot(vector, camFwdVec) * camFwdVec };

    return { glm::dot(cam->GetRightVector(), projectedVec), glm::dot(cam->GetUpVector(), projectedVec) };
  }
}

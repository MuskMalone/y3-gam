#include <pch.h>
#ifndef IMGUI_DISABLE
#include "Viewport.h"
#include <imgui/imgui.h>
#include <GUI/Helpers/AssetPayload.h>
#include "AssetBrowser.h"
#include <Events/EventManager.h>

//to remove
#include "Core/Camera.h"
#include "TempScene.h"
namespace GUI
{

    //namespace {
    //    auto MoveAlongPlane(float xDisplacement, float yDisplacement) {
    //        // Calculate the normal of the plane (eye to target direction)
    //        glm::vec3 normal = glm::normalize(m_target - m_eye);

    //        // Define the world up vector
    //        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    //        // Calculate the right and up vectors on the plane
    //        glm::vec3 right = glm::normalize(glm::cross(up, normal));  // Right vector
    //        glm::vec3 planeUp = glm::normalize(glm::cross(normal, right)); // True up vector on the plane

    //        // Calculate the movement vector along the plane
    //        m_sideMovementDelta = right * xDisplacement + planeUp * yDisplacement;

    //        //// Move the eye and target along the plane
    //        //m_eye += movement;
    //        //m_target += movement;
    //    }
    //}
  Viewport::Viewport(std::string const& name) : GUIWindow(name) {}

  void Viewport::Update(std::shared_ptr<Graphics::Framebuffer> const& framebuffer)
  {
    ImGui::Begin(mWindowName.c_str());
    ImGui::Image(
        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(framebuffer->GetColorAttachmentID())),
        ImGui::GetContentRegionAvail(),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );
    ////calculate the camera for moving
    ////replace the current camera data members with the new impl of camera 
    ////to xavier or whoever is doing this - tch
    //auto& cam = Scene::GetMainCamera();
    //// Check if the viewport is hovered
    //if (ImGui::IsItemFocused() || ImGui::IsItemHovered()) {
    //    // Check for left mouse button press
    //    if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left]) {
    //        // Get the mouse drag delta for the left button
    //        ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
    //        cam.m_rotationDelta.x += dragDelta.y;
    //        cam.m_rotationDelta.y -= dragDelta.x;
    //    }
    //    // Check for middle mouse button press
    //    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
    //        ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
    //        cam.MoveAlongPlane(dragDelta.x, dragDelta.y);
    //    }

    //    if (glm::abs(ImGui::GetIO().MouseWheel) > glm::epsilon<float>()) {
    //        cam.m_movementDelta = glm::normalize(cam.m_target - cam.m_eye) * ImGui::GetIO().MouseWheel;
    //    }
    //}

    // Reset the drag delta when the mouse button is released
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
    }

    
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

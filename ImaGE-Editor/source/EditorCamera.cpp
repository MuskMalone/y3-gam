#include <pch.h>
#include "EditorCamera.h"
#include "Input/InputManager.h"
#include <Graphics/Utils.h>

namespace Graphics {
  EditorCamera::EditorCamera(glm::vec3 position, float yaw, float pitch,
    float fov, float aspectRatio, float nearClip, float farClip) :
    CameraSpec(position, yaw, pitch, fov, aspectRatio, nearClip, farClip, true),
    mYaw{ yaw }, mPitch{ pitch }, mMoveSpeed{ sBaseMoveSpeed }, modified{ false }
  {

  }

  void EditorCamera::UpdateMatrices() {
    viewMatrix = Utils::Camera::GetViewMatrix(position, mYaw, mPitch);
    viewProjMatrix = GetProjMatrix() * viewMatrix;
    ComputeFrustum();
  }

  void EditorCamera::UpdateFromViewMtx(glm::mat4 const& newView) {
    // first get the inverse view mtx
    glm::mat4 const invView{ glm::inverse(viewMatrix) };
    glm::vec3 const front{ glm::normalize(-glm::vec3(invView[2])) };

    position = invView[3];
    mYaw = glm::degrees(atan2(front.z, front.x));
    mPitch = glm::degrees(asin(front.y));

    viewMatrix = newView;
    viewProjMatrix = GetProjMatrix() * viewMatrix;
    ComputeFrustum();
  }

  glm::mat4 EditorCamera::GetProjMatrix() const {
    return Utils::Camera::GetProjMatrix(fov, aspectRatio, nearClip, farClip);
  }

  //void EditorCamera::UpdateCamera(float dt) {

  //  if (Input::InputManager::IsKeyHeld(IK_W))
  //    ProcessKeyboardInput(CameraMovement::FORWARD, dt);
  //  if (Input::InputManager::IsKeyHeld(IK_S))
  //    ProcessKeyboardInput(CameraMovement::BACKWARD, dt);
  //  if (Input::InputManager::IsKeyHeld(IK_A))
  //    ProcessKeyboardInput(CameraMovement::LEFT, dt);
  //  if (Input::InputManager::IsKeyHeld(IK_D))
  //    ProcessKeyboardInput(CameraMovement::RIGHT, dt);
  //  if (Input::InputManager::IsKeyHeld(IK_Q))
  //    ProcessKeyboardInput(CameraMovement::DOWN, dt);
  //  if (Input::InputManager::IsKeyHeld(IK_E))
  //    ProcessKeyboardInput(CameraMovement::UP, dt);

  //  // Mouse input for looking around
  //  auto mouseDelta = Input::InputManager::GetInstance().GetMouseDelta();  // Get mouse delta from InputManager
  //  ProcessMouseInput(mouseDelta.x, mouseDelta.y);

  //  // Scroll input for zooming
  //  float scrollDelta = static_cast<float>(Input::InputManager::GetInstance().GetMouseScrollVert());  // Get scroll value
  //  ProcessMouseScroll(scrollDelta);
  //}

  glm::vec3 EditorCamera::GetForwardVector() const {
    return glm::normalize(glm::vec3(
      cos(glm::radians(mYaw)) * cos(glm::radians(mPitch)),
      sin(glm::radians(mPitch)),
      sin(glm::radians(mYaw)) * cos(glm::radians(mPitch))
    ));
  }

  glm::vec3 EditorCamera::GetRightVector() const {
    return glm::normalize(glm::cross(GetForwardVector(), glm::vec3{ 0.0f, 1.0f, 0.0f }));
  }

  glm::vec3 EditorCamera::GetUpVector() const {
    return glm::normalize(glm::cross(GetRightVector(), GetForwardVector()));
  }

  void EditorCamera::ProcessKeyboardInput(CameraMovement dir, float dt) {
    float const velocity = mMoveSpeed * dt;

    switch (dir) {
    case CameraMovement::FORWARD:
      position += GetForwardVector() * velocity;
      break;
    case CameraMovement::BACKWARD:
      position -= GetForwardVector() * velocity;
      break;
    case CameraMovement::LEFT:
      position -= GetRightVector() * velocity;
      break;
    case CameraMovement::RIGHT:
      position += GetRightVector() * velocity;
      break;
    case CameraMovement::DOWN:
      position -= glm::vec3{ 0.0f, 1.0f, 0.0f } *velocity;
      break;
    case CameraMovement::UP:
      position += glm::vec3{ 0.f, 1.f, 0.f } *velocity;
      break;
    default:
      break;
    }

    modified = true;
  }
  void EditorCamera::ProcessMouseInput(float offsetX, float offsetY) {
    offsetX *= sMouseSense;
    offsetY *= sMouseSense;

    mYaw += offsetX;
    mPitch -= offsetY;

    // Constrain pitch to avoid flipping the camera
    if (mPitch > 89.0f) mPitch = 89.0f;
    if (mPitch < -89.0f) mPitch = -89.0f;
    modified = true;
  }
  void EditorCamera::ProcessMouseScroll(float scrollOffset) {
    fov -= scrollOffset * sZoomSpeed;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
    modified = true;
  }

  void EditorCamera::MoveCamera(glm::vec3 const& offset) {
    position += offset;
    modified = true;
  }

  void EditorCamera::MoveAlongPlane(float xDisp, float yDisp) {
    glm::vec3 forward = GetForwardVector();
    forward.y = 0.0f; // zero out Y-component
    forward = glm::normalize(forward);

    glm::vec3 const rightMovement = GetRightVector() * xDisp * sMousePanningSpeed;
    glm::vec3 const forwardMovement = forward * yDisp * sMousePanningSpeed;

    // move based on inverted xDisp
    position += forwardMovement - rightMovement;
    modified = true;
  }

  void EditorCamera::InitForEditorView() {
    position = { 0.0f, 5.0f, 10.0f };
    mYaw = -90.0f;
    mPitch = 0.0f;  // look downwards slightly
    fov = 60.0f;
    aspectRatio = 16.f / 9.f;
    nearClip = 0.1f;
    farClip = 1500.0f;

    UpdateMatrices();
  }
}

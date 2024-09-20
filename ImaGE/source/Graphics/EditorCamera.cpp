#include <pch.h>
#include "EditorCamera.h"
#include "Input/InputManager.h"

namespace Graphics {
    glm::mat4 EditorCamera::GetViewMatrix() const {
        return Utils::Camera::GetViewMatrix(mPosition, mYaw, mPitch);
    }

    glm::mat4 EditorCamera::GetProjMatrix() const {
        return Utils::Camera::GetProjMatrix(mFov, mAspectRatio, mNearClip, mFarClip);
    }

    glm::mat4 EditorCamera::GetViewProjMatrix() const {
        return GetProjMatrix() * GetViewMatrix();
    }

    void EditorCamera::UpdateCamera(float dt) {

        if (Input::InputManager::IsKeyHeld(IK_W))
            ProcessKeyboardInput(CameraMovement::FORWARD, dt);
        if (Input::InputManager::IsKeyHeld(IK_S))
            ProcessKeyboardInput(CameraMovement::BACKWARD, dt);
        if (Input::InputManager::IsKeyHeld(IK_A))
            ProcessKeyboardInput(CameraMovement::LEFT, dt);
        if (Input::InputManager::IsKeyHeld(IK_D))
            ProcessKeyboardInput(CameraMovement::RIGHT, dt);
        if (Input::InputManager::IsKeyHeld(IK_Q))
            ProcessKeyboardInput(CameraMovement::DOWN, dt);
        if (Input::InputManager::IsKeyHeld(IK_E))
            ProcessKeyboardInput(CameraMovement::UP, dt);

        // Mouse input for looking around
        auto mouseDelta = Input::InputManager::GetInstance().GetMouseDelta();  // Get mouse delta from InputManager
        ProcessMouseInput(mouseDelta.x, mouseDelta.y);

        // Scroll input for zooming
        float scrollDelta = static_cast<float>(Input::InputManager::GetInstance().GetMouseScrollVert());  // Get scroll value
        ProcessMouseScroll(scrollDelta);
    }

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
        //if (Input::InputManager::GetInstance().IsKeyHeld(IK_W)) {
        //    mPosition += GetForwardVector() * mMoveSpeed * dt;
        //}
        //if (Input::InputManager::GetInstance().IsKeyHeld(IK_S)) {
        //    mPosition -= GetForwardVector() * mMoveSpeed * dt;
        //}
        //if (Input::InputManager::GetInstance().IsKeyHeld(IK_A)) {
        //    mPosition -= GetRightVector() * mMoveSpeed * dt;
        //}
        //if (Input::InputManager::GetInstance().IsKeyHeld(IK_D)) {
        //    mPosition += GetRightVector() * mMoveSpeed * dt;
        //}
        //if (Input::InputManager::GetInstance().IsKeyHeld(IK_Q)) {  // Move down
        //    mPosition -= glm::vec3{ 0.0f, 1.0f, 0.0f } *mMoveSpeed * dt;
        //}
        //if (Input::InputManager::GetInstance().IsKeyHeld(IK_E)) {  // Move up
        //    mPosition += glm::vec3{ 0.0f, 1.0f, 0.0f } *mMoveSpeed * dt;
        //}
        float velocity = mMoveSpeed * dt;

        if(dir == CameraMovement::FORWARD)
            mPosition += GetForwardVector() * velocity;
        if(dir == CameraMovement::BACKWARD)
            mPosition -= GetForwardVector() * velocity;
        if(dir == CameraMovement::LEFT)
            mPosition -= GetRightVector() * velocity;
        if(dir == CameraMovement::RIGHT)
            mPosition += GetRightVector() * velocity;
        if (dir == CameraMovement::DOWN)
            mPosition -= glm::vec3{ 0.0f, 1.0f, 0.0f } *velocity;
        if(dir == CameraMovement::UP)
            mPosition += glm::vec3{ 0.f, 1.f, 0.f } * velocity;

        //switch(dir) {
        //case CameraMovement::FORWARD:
        //    mPosition += GetForwardVector() * velocity;
        //    break;
        //case CameraMovement::BACKWARD:
        //    mPosition -= GetForwardVector() * velocity;
        //    break;
        //case CameraMovement::LEFT:
        //    mPosition -= GetRightVector() * velocity;
        //    break;
        //case CameraMovement::RIGHT:
        //    mPosition += GetRightVector() * velocity;
        //    break;
        //case CameraMovement::DOWN:
        //    mPosition -= glm::vec3{ 0.0f, 1.0f, 0.0f } * velocity;
        //    break;
        //case CameraMovement::UP:
        //    mPosition += glm::vec3{ 0.f, 1.f, 0.f} * velocity;
        //    break;
        //}
    }
    void EditorCamera::ProcessMouseInput(float offsetX, float offsetY) {
        offsetX *= mMouseSense;
        offsetY *= mMouseSense;

        mYaw += offsetX;
        mPitch += offsetY;

        // Constrain pitch to avoid flipping the camera
        if (mPitch > 89.0f) mPitch = 89.0f;
        if (mPitch < -89.0f) mPitch = -89.0f;
    }
    void EditorCamera::ProcessMouseScroll(float scrollOffset) {
        mFov -= scrollOffset * mZoomSpeed;
        if (mFov < 1.0f) mFov = 1.0f;
        if (mFov > 45.0f) mFov = 45.0f;
    }
}
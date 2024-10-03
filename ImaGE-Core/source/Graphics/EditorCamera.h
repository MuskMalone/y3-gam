#pragma once
#include "Utils.h"

namespace Graphics {
	class EditorCamera {

		enum class CameraMovement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			DOWN,
			UP
		};
	public:
		EditorCamera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),  // Default position
			float yaw = -90.0f,                               // Default yaw
			float pitch = 0.0f,                               // Default pitch
			float fov = 45.0f,                                // Default field of view
			float aspectRatio = 16.0f / 9.0f,                 // Default aspect ratio
			float nearClip = 0.1f,                            // Near clipping plane
			float farClip = 100.0f)                           // Far clipping plane
			: mPosition(position), mYaw(yaw), mPitch(pitch), mFov(fov),
			mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip) {}

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetViewProjMatrix() const;
		glm::vec3 GetPosition() const;

		void UpdateCamera(float dt);

		glm::vec3 GetForwardVector() const;
		glm::vec3 GetRightVector() const;
		glm::vec3 GetUpVector() const;

		void ProcessKeyboardInput(CameraMovement dir, float dt);
		void ProcessMouseInput(float offsetX, float offsetY);
		void ProcessMouseScroll(float scrollOffset);

		//tch : getter setters for visual testing
		void SetPosition(glm::vec3 const& p);
	private:
		glm::vec3 mPosition;
		float mYaw;
		float mPitch;
		float mFov;
		float mAspectRatio;
		float mNearClip;
		float mFarClip;

		float mMoveSpeed{10.f};
		float mMouseSense{ 0.f };
		float mZoomSpeed{5.f};
	};
}
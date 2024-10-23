/*!*********************************************************************
\file   EditorCamera.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The EditorCamera class supports camera movement through keyboard and mouse input, 
		allowing for translation, rotation, and zoom functionality. It provides methods 
		for calculating view, projection, and view-projection matrices.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/

#pragma once
#include "Utils.h"

namespace Graphics {
	class EditorCamera {
	public:
		enum class CameraMovement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			DOWN,
			UP
		};

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
		void SetPosition(glm::vec3 const& pos);

		void UpdateCamera(float dt);

		glm::vec3 GetForwardVector() const;
		glm::vec3 GetRightVector() const;
		glm::vec3 GetUpVector() const;

		void ProcessKeyboardInput(CameraMovement dir, float dt);
		void ProcessMouseInput(float offsetX, float offsetY);
		void ProcessMouseScroll(float scrollOffset);
		void MoveTowardsPoint(glm::vec3 const& target, float dt);
		void MoveAlongPlane(float xDisp, float yDisp);


	private:
		glm::vec3 mPosition;
		float mYaw;
		float mPitch;
		float mFov;
		float mAspectRatio;
		float mNearClip;
		float mFarClip;

		float mMoveSpeed{ 10.f };
		float mMousePanningSpeed{ 5.f };
		float mMouseSense{ 50.f };
		float mZoomSpeed{ 2.5f };
	};
}
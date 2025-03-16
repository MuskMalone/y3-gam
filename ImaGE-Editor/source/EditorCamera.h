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
#include <Graphics/Camera/CameraSpec.h>
#include <rttr/rttr_enable.h>

namespace Graphics {
	struct EditorCamera : public CameraSpec {
		enum class CameraMovement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			DOWN,
			UP
		};

		EditorCamera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), // Default position
			float yaw = -90.0f,                               // Default yaw
			float pitch = 0.0f,                               // Default pitch
			float fov = 45.0f,                                // Default field of view
			float aspectRatio = 16.0f / 9.0f,                 // Default aspect ratio
			float nearClip = 0.1f,                            // Near clipping plane
			float farClip = 1500.0f														// Far clipping plane
		);

		
		void UpdateMatrices();
		void UpdateFromViewMtx(glm::mat4 const& newView);
		//void UpdateCamera(float dt);

		glm::mat4 GetProjMatrix() const;
		glm::vec3 GetForwardVector() const;
		glm::vec3 GetRightVector() const;
		glm::vec3 GetUpVector() const;

		void ProcessKeyboardInput(CameraMovement dir, float dt);
		void ProcessMouseInput(float offsetX, float offsetY);
		void ProcessMouseScroll(float scrollOffset);

		/*!*********************************************************************
		\brief
		  Moves the camera based on an offset. This function is called by the
			viewport and already takes the delta time into account.
		\param target
			The amount to move
		************************************************************************/
		void MoveCamera(glm::vec3 const& offset);
		void MoveAlongPlane(float xDisp, float yDisp);
		
		// initializes camera for editor view
		void InitForEditorView();
		inline void Modified() noexcept{ modified = true; }

		float mYaw;
		float mPitch;
		float mMoveSpeed;
		bool modified;

	private:
		inline static float sBaseMoveSpeed{ 15.f };
		inline static float sMousePanningSpeed{ 5.f };
		inline static float sMouseSense{ 100.f };
		inline static float sZoomSpeed{ 2.5f };

		RTTR_ENABLE()
	};
}
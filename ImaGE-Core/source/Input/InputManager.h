#pragma once
/*!*********************************************************************
\file   InputManager.h
\author han.q\@digipen.edu
\date   19-September-2024
\brief
	Input Manager for Engine


	Setup:

	1. // Include this header file in your files
		 #include "../InputManager/InputManager.h"

	2. // Initialize the Input Manager in your Game's Init after Create Window
		 Input::InputManager::GetInstance().InitInputManager(ptr_window, width, height);

	3. // Call theUpdateInput() function at the start of each game loop ( Start of Update function )
		Input::InputManager::GetInstance().UpdateInput();

 ------------------------------------------------------------------------------------------------

	How to use:

	1. Get an instance of the Input Manager in your function/code:
		 Input::InputManager::GetInstance().InitInputManager(ptr_window, width, height);

	2. Access the function through the instance:
		 bool isTriggered = Input::InputManager::GetInstance().IsKeyTriggered(IK_MOUSE_LEFT);


Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/

#include "../Events/EventManager.h"
#include <glad/glad.h> // for access to OpenGL API declarations
#include <GLFW/glfw3.h>
#include <bitset>
#include "KeyCode.h"
#include <FrameRateController/FrameRateController.h>
#include <Singleton/ThreadSafeSingleton.h>
#include <array>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <External/GLFWwindowDestructor.h>

	namespace Input
	{
		using KEY_MAP = std::bitset<static_cast<size_t>(IK_KEY_COUNT)>;
		using KEY_PRESS_ARRAY = std::array<double, static_cast<size_t>(IK_KEY_COUNT)>;
		using vec2 = glm::vec2;

		class InputManager : public ThreadSafeSingleton<InputManager>
		{

		private:
			static int mWidth, mHeight;
			static double mKeyHeldTime;
			static double mScrollX, mScrollY;
			static glm::vec2 mCurrMousePos;
			static glm::vec2 mPrevMousePos;
			static KEY_MAP mKeyReleased;
			static KEY_MAP mKeyHeld;
			static KEY_MAP mKeysTriggered;
			static KEY_PRESS_ARRAY mKeyFramesHeld;
			static size_t mCurrFramebuffer; //!< id of curr buffer (for mouse to WorldSpace)

		public:
			/*!*********************************************************************
			\brief
				.Function to initialize the Input Manager (Call this function right after successfully initializing window)
			\params
				GLFWwindow* window
				pointer to current window
			\params
				int width
				width of the window
			\params
				int height
				height of the window
			\params
				double holdTime
				duration a key needs to be pressed to be recognized as a held
			************************************************************************/
			InputManager(std::unique_ptr<GLFWwindow, GLFWwindowDestructor>& window, int width, int height, double holdTime = 0.5);

			/*!*********************************************************************
			\brief
				.Function to check if a key is held
			\params
				KEY_CODE key
				Enum of the key you want to check
			\return
				returns true is key is held
			************************************************************************/
			static bool IsKeyHeld(KEY_CODE);

			/*!*********************************************************************
			\brief
				.Function to check if a key is released
			\params
				KEY_CODE key
				Enum of the key you want to check
			\return
				returns true is key is relased in this frame
			************************************************************************/
			static bool IsKeyReleased(KEY_CODE);

		
			/*!*********************************************************************
			\brief
				.Function to check if a key is triggered
			\params
				KEY_CODE key
				Enum of the key you want to check
			\return
				returns true is key is triggered
			************************************************************************/
			static bool IsKeyTriggered(KEY_CODE);

			/*!*********************************************************************
			\brief
				.Function to check if a key is Pressed
			\params
				KEY_CODE key
				Enum of the key you want to check
			\return
				returns true is key is Pressed
			************************************************************************/
			static bool IsKeyPressed(KEY_CODE);


			/*!*********************************************************************
			\brief
				.Function to check the yoffset of the mouse scroll for the current frame
			\return
				yoffset of mouse scroll
			************************************************************************/
			static double GetMouseScrollVert();


			/*!*********************************************************************
			\brief
			  .Function to check the xoffset of the mouse scroll for the current frame
			\return
				xoffset of  mouse scroll
			************************************************************************/
			static double GetMouseScrollHor();



			/*!*********************************************************************
			\brief
				.Function to get the distance travelled by the mouse
			\return
				xoffset of  mouse scroll
			************************************************************************/
			static vec2 GetMouseDelta();

			/*!*********************************************************************
			\brief
				Sets the new dimensions to compute the input with
			  
			\param width
				New input region width

			\param height
				New input region height

			\return
			************************************************************************/
			void SetDim(int width, int height);

			/*!*********************************************************************
			\brief
				.Function to update the Input for this frame. It will call GLFWPollEvents to get all key triggers and update the btiset

			************************************************************************/
			void UpdateInput();

			/*!*********************************************************************
			\brief
				This function dispatches the relevant input events based on the
				updated key states after polling

			************************************************************************/
			void QueueInputEvents();

			/*!*********************************************************************
			\brief
				.Function to get the mouse pos in the current frame

			\return
				returns the mouse's pos as dvec2
			************************************************************************/
			static vec2 GetMousePos();

			/*!*********************************************************************
			\brief
			  Sets the current framebuffer. this ID is for GetMousePosWorld()
					(editor camera position and size differs from game one)
			\params
			  framebufferID
			\return
			************************************************************************/
			void SetCurrFramebuffer(size_t framebufferID);

			/*!*********************************************************************
		\brief
			.Function to get the mouse pos in the current frame

		\return
			returns the mouse's pos as dvec2
		************************************************************************/
			static vec2 GetMousePosWorld();

			/*!*********************************************************************
		\brief
			.This callback function is called when the user triggers/release/hold a keyboard key
		\params
			GLFWwindow* window
			pointer to current window
		\params
			int key
			keyboard key that was pressed or released
		\params
			int scanCode
			scancode of the key (Platform-specific)
		\params
			int action
			action is either GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
		\params
			int mod
			bit_field representing which modifier keys (shift, alt, control) were held down
		************************************************************************/
			static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mod);

			/*!*********************************************************************
			\brief
				.This callback function is called when the user move the mouse
			\params
				GLFWwindow* window
				pointer to current window
			\params
				double xpos
				 cursor x-coordinate, relative to the left edge of the window
			\params
				double ypos
				cursor y-coordinate, relative to the left edge of the window
			************************************************************************/
			static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);

			/*!*********************************************************************
			\brief
				.This callback function is called when the user clicks a mouse button
			\params
				GLFWwindow* window
				pointer to current window
			\params
				int button
				mouse button that was pressed or released
			\params
				int action
				action is either GLFW_PRESS or GLFW_RELEASE
			\params
				int mod
				bit_field representing which modifier keys (shift, alt, control) were held down
			************************************************************************/
			static void MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod);

			/*!*********************************************************************
			\brief
				.This callback function is called when the user scrolls, whether with a mouse wheel or touchpad gesture
			\params
				GLFWwindow* window
				pointer to current window
			\params
				double
				Scroll offset along X-axis
			\params
				double yoffset
				Scroll offset along Y-axis
			************************************************************************/
			static void MouseScrollCallback(GLFWwindow* pwin, double xoffset, double yoffset);

			//void TestInputManager();

		};


	}



// OLD FUNCTIONS ARCHIVE
/*template <typename T>
void CheckAndDispatch(KEY_CODE code)
{
	if (IsKeyTriggered(code))
	{
		GE::Events::EventManager::GetInstance().Dispatch(T(code));
	}
}*/
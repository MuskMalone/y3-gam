/*!*********************************************************************
\file   InputManager.cpp
\author han.q\@digipen.edu
\date   19-September-2024
\brief
	Input Manager for Engine


	Setup:

	1. // Include this header file in your files
		 #include "../InputManager/InputManager.h"

	2. // Initialize the Input Manager in your Game's Init after Create Window
		 Input::InputManager::GetInstance().InitInputManager(ptr_window, width, height);mMousePos

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
#include <pch.h>
#include <External/GLFWwindowDestructor.h>
#ifndef IMGUI_DISABLE
#include <ImGui/backends/imgui_impl_glfw.h>
#endif
#include "InputManager.h"
#include <Events/EventManager.h>
#include <FrameRateController/FrameRateController.h>

using namespace Input;
using namespace Events;

int InputManager::mWidth;
double InputManager::mScrollX;
double InputManager::mScrollY;
int InputManager::mHeight;
double InputManager::mKeyHeldTime;
glm::vec2 InputManager::mPrevMousePos;
glm::vec2 InputManager::mCurrMousePos;
KEY_MAP InputManager::mKeyReleased;
KEY_MAP InputManager::mKeyHeld;
KEY_MAP InputManager::mKeysTriggered;
KEY_PRESS_ARRAY InputManager::mKeyFramesHeld;
size_t InputManager::mCurrFramebuffer;

void InputManager::InitInputManager(std::unique_ptr<GLFWwindow, GLFWwindowDestructor>& window, int width, int height, double holdTime)
{
	mHeight = height;
	mWidth = width;
	mKeyHeld.reset();
	mKeysTriggered.reset();
	mKeyFramesHeld.fill(0);
	mKeyHeldTime = holdTime;
	
	// Subscribe to the mouse/keyboard event
	glfwSetKeyCallback(window.get(), KeyCallback);
	glfwSetCursorPosCallback(window.get(), MousePosCallback);
	glfwSetMouseButtonCallback(window.get(), MouseButtonCallback);
	glfwSetScrollCallback(window.get(), MouseScrollCallback);
}

void InputManager::SetDim(int width, int height)
{
	mHeight = height;
	mWidth = width;
}

void InputManager::UpdateInput()
{
	mPrevMousePos = mCurrMousePos;
	mKeyReleased.reset();
	mKeysTriggered.reset();
	mScrollX = mScrollY = 0;
	glfwPollEvents();
	double dt = FrameRateController::GetInstance().GetDeltaTime();
	for (int i{ 0 }; i < static_cast<int>(IK_KEY_COUNT); ++i)
	{

		mKeyFramesHeld[i] = (mKeyReleased[i]) ? 0: (mKeyFramesHeld[i] > 0.f || mKeysTriggered[i]) ? (mKeyFramesHeld[i] < mKeyHeldTime) ? mKeyFramesHeld[i] + dt: mKeyFramesHeld[i]: 0;
		mKeyHeld[i] = (mKeyFramesHeld[i] >= mKeyHeldTime);
	}

	QueueInputEvents();
}



void InputManager::QueueInputEvents()
{
	if (IsKeyHeld(IK_MOUSE_LEFT))
	{
		//eventMan.Subscribe();
		QUEUE_EVENT(Events::MouseHeldEvent, IK_MOUSE_LEFT);
	}
	if (IsKeyTriggered(IK_MOUSE_LEFT))
	{
		QUEUE_EVENT(Events::MouseTriggeredEvent, IK_MOUSE_LEFT);
	}
	else if (IsKeyReleased(IK_MOUSE_LEFT))
	{
		QUEUE_EVENT(Events::MouseReleasedEvent, IK_MOUSE_LEFT);
	}

	if (IsKeyHeld(IK_H))
	{
		QUEUE_EVENT(Events::KeyHeldEvent, IK_H);
	}
	if (IsKeyHeld(IK_J))
	{
		QUEUE_EVENT(Events::KeyHeldEvent, IK_J);
	}
	if (IsKeyTriggered(IK_H))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_H);
#ifndef IMGUI_DISABLE
		Debug::DebugLogger::GetInstance().LogInfo("Testies");
		throw Debug::Exception<InputManager>(Debug::LVL_CRITICAL, Msg("ThrowTesties"));
#endif
	}

	if (IsKeyTriggered(IK_K))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_K);
	}
	if (IsKeyTriggered(IK_E))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_E);
	}
	if (IsKeyTriggered(IK_Q))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_Q);
	}
	if (IsKeyTriggered(IK_R))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_R);
	}
	if (IsKeyTriggered(IK_ESCAPE))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_ESCAPE);
	}
	if (IsKeyTriggered(IK_DELETE))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_DELETE);
	}
	if (IsKeyTriggered(IK_F11))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_F11);
	}
	if (IsKeyTriggered(IK_M)) // TEST FOR PAUSE SO FAR
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_M);
	}
	if (IsKeyTriggered(IK_1))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_1);
	}
	if (IsKeyTriggered(IK_2))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, IK_2);
	}
	// ALT TAB = MINIMIZE
	if (IsKeyPressed(IK_LEFT_ALT) && IsKeyTriggered(IK_TAB))
	{
		QUEUE_EVENT(Events::WindowMinimized);
		//eventMan.Dispatch(WindowMinimize());
	}
}

bool InputManager::IsKeyTriggered(KEY_CODE key)
{
	return (mKeysTriggered[static_cast<int>(key)]);
}
bool InputManager::IsKeyHeld(KEY_CODE key)
{
	return (mKeyHeld[static_cast<int>(key)]);
}
bool InputManager::IsKeyReleased(KEY_CODE key)
{
	return (mKeyReleased[static_cast<int>(key)]);
}
bool InputManager::IsKeyPressed(KEY_CODE key)
{
	return (mKeyFramesHeld[static_cast<int>(key)] > 0.f);
}



vec2  InputManager::GetMousePos()
{
	return mCurrMousePos;
}

void Input::InputManager::SetCurrFramebuffer(size_t framebufferID)
{
	mCurrFramebuffer = framebufferID;
}

vec2 InputManager::GetMousePosWorld()
{
	//auto& gEngine{ Graphics::GraphicsEngine::GetInstance() };
	//// TODO: change to current framebuffer
	//Graphics::gVec2 worldPosF32{ gEngine.ScreenToWS({ static_cast<GLfloat>(mMousePos.x), static_cast<GLfloat>(mHeight - mMousePos.y) }, m_currFramebuffer) };
	//return  {worldPosF32.x, worldPosF32.y};
	return { 0,0 };
}


double InputManager::GetMouseScrollVert()
{
	return mScrollY;
}

double InputManager::GetMouseScrollHor()
{
	return mScrollX;
}

vec2 InputManager::GetMouseDelta()
{
	return (mCurrMousePos - mPrevMousePos);
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mod)
{
	UNREFERENCED_PARAMETER(scanCode);
	UNREFERENCED_PARAMETER(mod);

#ifndef IMGUI_DISABLE
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
	{
		ImGui_ImplGlfw_KeyCallback(window, key, scanCode, action, mod);
	}
#else
	UNREFERENCED_PARAMETER(window);
#endif

	// returns -1 when keyboard functions such as change laptop brightness happens
	if (key < 0)
		return;

	mKeyReleased[key] = (GLFW_RELEASE == action);
	mKeysTriggered[key] = (GLFW_PRESS == action);
}

// Mouse callback function
void InputManager::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
#ifndef IMGUI_DISABLE
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#else
	UNREFERENCED_PARAMETER(window);
#endif
	
	mCurrMousePos.x = static_cast<float>(xpos);
	mCurrMousePos.y = static_cast<float>(ypos);


}

void InputManager::MouseButtonCallback(GLFWwindow* pwin, int button, int action, int mod)
{
	UNREFERENCED_PARAMETER(pwin);
	UNREFERENCED_PARAMETER(mod);

#ifndef IMGUI_DISABLE
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
	{
		ImGui_ImplGlfw_MouseButtonCallback(pwin, button, action, mod);
	}
#endif



	mKeyReleased[button] = (GLFW_RELEASE == action);
	mKeysTriggered[button] = (GLFW_PRESS == action);

}

void InputManager::MouseScrollCallback(GLFWwindow* pwin, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(pwin);
	mScrollX = xoffset;
	mScrollY = yoffset;

#ifndef IMGUI_DISABLE
	ImGui_ImplGlfw_ScrollCallback(pwin, xoffset, yoffset);
#endif
	//y_off = ((y_off + yoffset) > 4) ? 4 : ((y_off + yoffset) < -4) ? -4 : y_off + yoffset;
	// << y_off << "\n";
	////#ifdef _DEBUG
	////   << "Mouse scroll wheel offset: ("
	////    << xoffset << ", " << yoffset << ")" << std::endl;
	////#endif
}


//void InputManager::TestInputManager() {
//	InputManager* im = &(GE::Input::InputManager::GetInstance());
//	if (im->IsKeyTriggered(IK_A)) {
//		 << "Key A is Triggered\n";
//	}
//	if (im->IsKeyHeld(IK_A)) {
//		 << "Key A is Held\n";
//	}
//	if (im->IsKeyReleased(IK_A)) {
//		 << "Key A is Released\n";
//	}
//
//	if (im->IsKeyPressed(IK_MOUSE_LEFT)) {
//		 << "Mouse Pos: " << im->GetMousePosWorld().x << "," << im->GetMousePosWorld().y << "\n";
//	}
//}


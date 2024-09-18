/*!*********************************************************************
\file   InputManager.cpp
\author han.q\@digipen.edu
\date   28-September-2023
\brief
	Input Manager for Engine


	Setup:

	1. // Include the header file in your files
		 #include "../InputManager/InputManager.h"

	2. // Initialize the Input Manager in your Game's Init after Create Window
		 GE::Input::InputManager* im = &(GE::Input::InputManager::GetInstance());
		 im->InitInputManager(GLHelper::ptr_window, GLHelper::width, GLHelper::height);

	3. // Call theUpdateInput() function at the start of each game loop ( Start of Update function )
			GE::Input::InputManager* im = &(GE::Input::InputManager::GetInstance());
			im->UpdateInput();

 ------------------------------------------------------------------------------------------------

	How to use:

	1. Get an instance of the Input Manager in your function/code:
		 im->InitInputManager(GLHelper::ptr_window, GLHelper::width, GLHelper::height);

	2. Access the function through the instance:
		 bool isTriggered = im->IsKeyTriggered(GPK_MOUSE_LEFT);


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

int InputManager::m_width;
double InputManager::m_scrollX;
double InputManager::m_scrollY;
int InputManager::m_height;
double InputManager::m_keyHeldTime;
glm::vec2 InputManager::m_mousePos;
KEY_MAP InputManager::m_keyReleased;
KEY_MAP InputManager::m_keyHeld;
KEY_MAP InputManager::m_keysTriggered;
KEY_PRESS_ARRAY InputManager::m_keyFramesHeld;
size_t InputManager::m_currFramebuffer;

void InputManager::InitInputManager(std::unique_ptr<GLFWwindow, GLFWwindowDestructor>& window, int width, int height, double holdTime)
{
	m_height = height;
	m_width = width;
	m_keyHeld.reset();
	m_keysTriggered.reset();
	m_keyFramesHeld.fill(0);
	m_keyHeldTime = holdTime;
	
	// Subscribe to the mouse/keyboard event
	glfwSetKeyCallback(window.get(), KeyCallback);
	glfwSetCursorPosCallback(window.get(), MousePosCallback);
	glfwSetMouseButtonCallback(window.get(), MouseButtonCallback);
	glfwSetScrollCallback(window.get(), MouseScrollCallback);
}

void InputManager::SetDim(int width, int height)
{
	m_height = height;
	m_width = width;
}

void InputManager::UpdateInput()
{
	m_keyReleased.reset();
	m_keysTriggered.reset();
	m_scrollX = m_scrollY = 0;
	glfwPollEvents();
	double dt = FrameRateController::GetInstance().GetDeltaTime();
	for (int i{ 0 }; i < static_cast<int>(GPK_KEY_COUNT); ++i)
	{

		m_keyFramesHeld[i] = (m_keyReleased[i]) ? 0: (m_keyFramesHeld[i] > 0.f || m_keysTriggered[i]) ? (m_keyFramesHeld[i] < m_keyHeldTime) ? m_keyFramesHeld[i] + dt: m_keyFramesHeld[i]: 0;
		m_keyHeld[i] = (m_keyFramesHeld[i] >= m_keyHeldTime);
	}

	QueueInputEvents();
}



void InputManager::QueueInputEvents()
{
	if (IsKeyHeld(GPK_MOUSE_LEFT))
	{
		//eventMan.Subscribe();
		QUEUE_EVENT(Events::MouseHeldEvent, GPK_MOUSE_LEFT);
	}
	if (IsKeyTriggered(GPK_MOUSE_LEFT))
	{
		QUEUE_EVENT(Events::MouseTriggeredEvent, GPK_MOUSE_LEFT);
	}
	else if (IsKeyReleased(GPK_MOUSE_LEFT))
	{
		QUEUE_EVENT(Events::MouseReleasedEvent, GPK_MOUSE_LEFT);
	}

	if (IsKeyHeld(GPK_H))
	{
		QUEUE_EVENT(Events::KeyHeldEvent, GPK_H);
	}
	if (IsKeyHeld(GPK_J))
	{
		QUEUE_EVENT(Events::KeyHeldEvent, GPK_J);
	}
	if (IsKeyTriggered(GPK_H))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_H);
	}

	if (IsKeyTriggered(GPK_K))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_K);
	}
	if (IsKeyTriggered(GPK_E))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_E);
	}
	if (IsKeyTriggered(GPK_Q))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_Q);
	}
	if (IsKeyTriggered(GPK_R))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_R);
	}
	if (IsKeyTriggered(GPK_ESCAPE))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_ESCAPE);
	}
	if (IsKeyTriggered(GPK_DELETE))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_DELETE);
	}
	if (IsKeyTriggered(GPK_F11))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_F11);
	}
	if (IsKeyTriggered(GPK_M)) // TEST FOR PAUSE SO FAR
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_M);
	}
	if (IsKeyTriggered(GPK_1))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_1);
	}
	if (IsKeyTriggered(GPK_2))
	{
		QUEUE_EVENT(Events::KeyTriggeredEvent, GPK_2);
	}
	// ALT TAB = MINIMIZE
	if (IsKeyPressed(GPK_LEFT_ALT) && IsKeyTriggered(GPK_TAB))
	{
		QUEUE_EVENT(Events::WindowMinimized);
		//eventMan.Dispatch(WindowMinimize());
	}
}

bool InputManager::IsKeyTriggered(KEY_CODE key)
{
	return (m_keysTriggered[static_cast<int>(key)]);
}
bool InputManager::IsKeyHeld(KEY_CODE key)
{
	return (m_keyHeld[static_cast<int>(key)]);
}
bool InputManager::IsKeyReleased(KEY_CODE key)
{
	return (m_keyReleased[static_cast<int>(key)]);
}
bool InputManager::IsKeyPressed(KEY_CODE key)
{
	return (m_keyFramesHeld[static_cast<int>(key)] > 0.f);
}



vec2  InputManager::GetMousePos()
{
	return m_mousePos;
}

void Input::InputManager::SetCurrFramebuffer(size_t framebufferID)
{
	m_currFramebuffer = framebufferID;
}

vec2 InputManager::GetMousePosWorld()
{
	//auto& gEngine{ Graphics::GraphicsEngine::GetInstance() };
	//// TODO: change to current framebuffer
	//Graphics::gVec2 worldPosF32{ gEngine.ScreenToWS({ static_cast<GLfloat>(m_mousePos.x), static_cast<GLfloat>(m_height - m_mousePos.y) }, m_currFramebuffer) };
	//return  {worldPosF32.x, worldPosF32.y};
	return { 0,0 };
}


double InputManager::GetMouseScrollVert()
{
	return m_scrollY;
}

double InputManager::GetMouseScrollHor()
{
	return m_scrollX;
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

	m_keyReleased[key] = (GLFW_RELEASE == action);
	m_keysTriggered[key] = (GLFW_PRESS == action);
}

// Mouse callback function
void InputManager::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
#ifndef IMGUI_DISABLE
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#else
	UNREFERENCED_PARAMETER(window);
#endif
	m_mousePos.x = static_cast<float>(xpos);
	m_mousePos.y = static_cast<float>(ypos);

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

	m_keyReleased[button] = (GLFW_RELEASE == action);
	m_keysTriggered[button] = (GLFW_PRESS == action);

}

void InputManager::MouseScrollCallback(GLFWwindow* pwin, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(pwin);
	m_scrollX = xoffset;
	m_scrollY = yoffset;

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
//	if (im->IsKeyTriggered(GPK_A)) {
//		 << "Key A is Triggered\n";
//	}
//	if (im->IsKeyHeld(GPK_A)) {
//		 << "Key A is Held\n";
//	}
//	if (im->IsKeyReleased(GPK_A)) {
//		 << "Key A is Released\n";
//	}
//
//	if (im->IsKeyPressed(GPK_MOUSE_LEFT)) {
//		 << "Mouse Pos: " << im->GetMousePosWorld().x << "," << im->GetMousePosWorld().y << "\n";
//	}
//}


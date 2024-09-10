/*!*********************************************************************
\file   InputAssistant.h
\author chengen.lau\@digipen.edu
\date   11-June-2024
\brief  Definition of InputAssistant class, which functions as a
        small-scale input manager for the program. An interested
        class must first register a key press or hold event via
        functions RegisterKeyPressEvent or RegisterKeyHoldEvent,
        passing in the key code and relevant function for the event.

        The InputAssistant will now be responsible for invoking
        the relevant function based on the key state.
  
 
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>

class InputAssistant
{
public:
  using InputEventCallback = std::function<void()>;
  using CursorEventCallback = std::function<void(double, double)>;

  static void Init(GLFWwindow* window);
  static void Update();

  static void RegisterKeyPressEvent(int key, InputEventCallback func);
  static void RegisterKeyReleaseEvent(int key, InputEventCallback func);
  static void RegisterCursorEvent(CursorEventCallback func);

private:
  static std::unordered_map<int, std::pair<bool, std::vector<InputEventCallback>>> m_keyPressStates;
  static std::unordered_map<int, std::pair<bool, std::vector<InputEventCallback>>> m_keyReleasedStates;
  static std::vector<CursorEventCallback> m_cursorEventCallbacks;
  static constexpr float KEY_HOLD_DELAY = 0.1f;

  /*========== CALLBACK FUNCTIONS ==========*/
  /*
      This function serves as the callback parameter for
        glfwSetKeyCallback function used in the main function

      Esc - close the app
  */
  static void KeyCallback(GLFWwindow* pWindow, int key, int scanCode, int action, int mods);

  /*
      This function serves as the callback parameter for
        glfwSetMouseButtonCallback function used in the main function
  */
  static void MouseButtonCallback(GLFWwindow* pWindow, int key, int action, int mods);

  /*
      This function serves as the callback parameter for
        glfwSetCursorPosCallback function used in the main function
  */
  static void CursorPosCallback(GLFWwindow* pWindow, double xPos, double yPos);

  /*
      This function serves as the callback parameter for
        glfwSetScrollCallback function used in the main function
  */
  static void ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset);

  /*
      This function serves as the callback parameter for
        glfwSetWindowSizeCallback function used in the main function
  */
  static void SizeCallback(GLFWwindow* pWindow, int width, int height);
};

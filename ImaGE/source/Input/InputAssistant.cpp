#include <pch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "InputAssistant.h"

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

std::unordered_map<int, std::pair<bool, std::vector<InputAssistant::InputEventCallback>>> InputAssistant::m_keyPressStates;
std::unordered_map<int, std::pair<bool, std::vector<InputAssistant::InputEventCallback>>> InputAssistant::m_keyReleasedStates;
std::vector<InputAssistant::CursorEventCallback> InputAssistant::m_cursorEventCallbacks;

void InputAssistant::RegisterKeyPressEvent(int key, InputEventCallback func)
{
  auto& list{ m_keyPressStates[key] };
  list.first = false;
  list.second.emplace_back(func);
}

void InputAssistant::RegisterKeyReleaseEvent(int key, InputEventCallback func)
{
  auto& list{ m_keyReleasedStates[key] };
  list.first = false;
  list.second.emplace_back(func);
}

void InputAssistant::RegisterCursorEvent(CursorEventCallback func)
{
  m_cursorEventCallbacks.emplace_back(func);
}

void InputAssistant::Init(GLFWwindow* window)
{
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetScrollCallback(window, ScrollCallback);
  glfwSetWindowSizeCallback(window, SizeCallback);
}

void InputAssistant::Update()
{
  for (auto& [key, trigger] : m_keyPressStates)
  {
    if (!trigger.first) { continue; }

    for (auto& func : trigger.second)
    {
      func();
    }
  }

  for (auto& [key, trigger] : m_keyReleasedStates)
  {
    if (!trigger.first) { continue; }

    for (auto& func : trigger.second)
    {
      func();
    }
  }
}

void InputAssistant::KeyCallback(GLFWwindow* pWindow, int key, int scanCode, int action, int mods)
{
  {
    auto pressSubscribers{ m_keyPressStates.find(key) };
    if (pressSubscribers != m_keyPressStates.end())
    {
      pressSubscribers->second.first = action != GLFW_RELEASE;
    }
  }

  auto releaseSubscribers{ m_keyReleasedStates.find(key) };
  if (releaseSubscribers != m_keyReleasedStates.end())
  {
    releaseSubscribers->second.first = action == GLFW_RELEASE;
  }

  ImGui_ImplGlfw_KeyCallback(pWindow, key, scanCode, action, mods);
}

void InputAssistant::MouseButtonCallback(GLFWwindow* pWindow, int key, int action, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
  {
    ImGui_ImplGlfw_MouseButtonCallback(pWindow, key, action, mods);
  }
  else
  {
    {
      auto pressSubscribers{ m_keyPressStates.find(key) };
      if (pressSubscribers != m_keyPressStates.end())
      {
        pressSubscribers->second.first = action != GLFW_RELEASE;
      }
    }

    auto releaseSubscribers{ m_keyReleasedStates.find(key) };
    if (releaseSubscribers != m_keyReleasedStates.end())
    {
      releaseSubscribers->second.first = action == GLFW_RELEASE;
    }
  }
}

void InputAssistant::CursorPosCallback(GLFWwindow* pWindow, double xPos, double yPos)
{
  for (auto& fn : m_cursorEventCallbacks) { fn(xPos, yPos); }

  ImGui_ImplGlfw_CursorPosCallback(pWindow, xPos, yPos);
}


void InputAssistant::ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset)
{


  ImGui_ImplGlfw_ScrollCallback(pWindow, xOffset, yOffset);
}

void InputAssistant::SizeCallback(GLFWwindow* pWindow, int width, int height)
{
  UNREFERENCED_PARAMETER(pWindow);
  UNREFERENCED_PARAMETER(width);
  UNREFERENCED_PARAMETER(height);
}

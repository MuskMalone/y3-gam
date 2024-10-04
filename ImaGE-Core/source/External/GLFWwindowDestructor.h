/*!*********************************************************************
\file   GLFWwindowDestructor.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Struct to act as the deleter for a std::unique_ptr<GLFWwindow>
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GLFWwindowDestructor {
  void operator()(GLFWwindow* ptr) { glfwDestroyWindow(ptr); }
};

/*!*********************************************************************
\file   pch.h
\date   5-October-2024
\brief  Pre-compiled header for the project
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once

#define UNREFERENCED_PARAMETER(P) (P)
#pragma warning(disable : 4005)
#pragma warning(disable : 5105) // macro expansion producing 'defined' has undefined behavior

// stdlib
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <array>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iostream>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <cstdarg>
#include <thread>
#include <type_traits>

#include <FrameRateController/FrameRateController.h>
#include <DebugTools/Exception/Exception.h>			      //Exception Class for Logging
#include <DebugTools/DebugLogger/DebugLogger.h>       //Logging Tool

#include "PxPhysicsAPI.h"

#include <Globals.h>

// glad
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#ifdef _DEBUG
#include <Debug.h>
#endif
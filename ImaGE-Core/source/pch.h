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

// jolt
#include <Jolt/Jolt.h>                                // General Jolt Physics include
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>               // Physics system (core of the simulation)
#include <Jolt/Physics/Body/Body.h>                   // Rigid bodies
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // Settings for creating bodies
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>       // Base shape class
#include <Jolt/Physics/Collision/PhysicsMaterial.h>   // Material properties like friction and restitution
#include <Jolt/Core/Reference.h>                      // Smart pointer (Ref<T>)
#include <Jolt/Math/Vec3.h>                           // 3D vector math (Vec3)
#include <Jolt/Math/Quat.h>                           // Quaternion math (Quat)
#include <Jolt/Physics/Collision/CollisionGroup.h>    // Collision layer and group information
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>

#include <FrameRateController/FrameRateController.h>
#include <DebugTools/Exception/Exception.h>			      //Exception Class for Logging
#include <DebugTools/DebugLogger/DebugLogger.h>       //Logging Tool

#include <Globals.h>

// glad
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef _DEBUG
#include <Debug.h>
#endif
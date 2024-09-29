#pragma once

#define UNREFERENCED_PARAMETER(P) (P)
#define _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS
#pragma warning(disable : 4005)
//stdlib
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
//#include <sstream>
//#include <fstream>
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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PxPhysicsAPI.h"	

#include <DebugTools/Exception/Exception.h>			//Exception Class for Logging
#include <DebugTools/DebugLogger/DebugLogger.h> //Logging Tool

#include <Globals.h>

//glad
#include <glad/glad.h>

#ifdef _DEBUG
#include <Debug.h>
#endif

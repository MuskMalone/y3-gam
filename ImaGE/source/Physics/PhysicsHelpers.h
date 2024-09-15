#pragma once
#include <pch.h>

namespace IGE {
	namespace Physics {
		inline glm::vec3 ToGLMVec3(JPH::Vec3 const& v) {
			return glm::vec3{v.GetX(), v.GetY(), v.GetZ() };
		}
		inline JPH::Vec3 ToJPHVec3(glm::vec3 const& v){
			return JPH::Vec3(v.x, v.y, v.z);
		}
	}
}
#pragma once
#include <pch.h>

namespace IGE {
	namespace Physics {
		inline glm::vec3 ToGLMVec3(physx::PxVec3 const& v) {
			return glm::vec3{v.x, v.y, v.z };
		}
		inline physx::PxVec3 ToPxVec3(glm::vec3 const& v){
			return physx::PxVec3(v.x, v.y, v.z);
		}
		inline physx::PxVec3 ToPhysicsUnits(physx::PxVec3 const& v) {
			return v / 2.f;
		}
		inline glm::vec3 ToPhysicsUnits(glm::vec3 const& v) {
			return v / 2.f;
		}
		inline physx::PxVec3 ToGraphicUnits(physx::PxVec3 const& v) {
			return v * 2.f;
		}
		inline glm::vec3 ToGraphicUnits(glm::vec3 const& v) {
			return v * 2.f;
		}
		//inline JPH::Vec3 ToJPHVec3(glm::dvec3 const& v) {
		//	return JPH::Vec3(static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z));
		//}
		
	}
}
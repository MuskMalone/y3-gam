#pragma once
#include <pch.h>
#include <glm/gtc/quaternion.hpp>
#include <Core/Entity.h>
namespace IGE {
	namespace Physics {
		inline glm::vec3 ToGLMVec3(physx::PxVec3 const& v) {
			return glm::vec3{v.x, v.y, v.z };
		}
		inline physx::PxVec3 ToPxVec3(glm::vec3 const& v){
			return physx::PxVec3(v.x, v.y, v.z);
		}
		inline glm::quat ToGLMQuat(physx::PxQuat const& q) {
			return glm::quat{q.w, q.x, q.y, q.z};  // GLM uses {w, x, y, z}
		}
		inline physx::PxQuat ToPxQuat(glm::quat const& q) {
			return physx::PxQuat{q.x, q.y, q.z, q.w};  // PhysX uses {x, y, z, w}
		}

		template <typename T>
		inline T ToPhysicsUnits(T const& v) {
			return v / 2.f;
		}
		template <typename T>
		inline T ToGraphicUnits(T const& v) {
			return v * 2.f;
		}


		
	}
}
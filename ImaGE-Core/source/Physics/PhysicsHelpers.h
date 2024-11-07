#pragma once
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
		inline glm::quat ToGLMQuat(const physx::PxQuat& q) {
			// Only negate the z component to adjust for the inverted Z-axis
			return glm::quat(q.w, q.x, q.y, -q.z);
		}
		// Convert PxQuat to Euler angles (roll, pitch, yaw) using GLM
		inline physx::PxVec3 ToPxEuler(const physx::PxQuat& quat) {
			// Convert PhysX quaternion to GLM quaternion
			glm::quat glmQuat(quat.w, quat.x, quat.y, quat.z);

			// Convert GLM quaternion to Euler angles
			glm::vec3 euler = glm::eulerAngles(glmQuat);

			// Return Euler angles with inverted Z-axis for PhysX
			return physx::PxVec3(euler.x, euler.y, -euler.z);  // Invert Z to match PhysX coordinate system
		}

		// Convert Euler angles (roll, pitch, yaw) to PxQuat using GLM
		inline physx::PxQuat ToPxQuat(const physx::PxVec3& euler) {
			// Create GLM Euler angles with inverted Z-axis
			glm::vec3 glmEuler(euler.x, euler.y, -euler.z);

			// Convert Euler angles to GLM quaternion
			glm::quat glmQuat = glm::quat(glmEuler);

			// Convert GLM quaternion to PhysX quaternion
			return physx::PxQuat(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
		}
		inline physx::PxQuat ToPxQuat(const glm::quat& q) {
			// Only negate the z component to adjust for the inverted Z-axis
			return physx::PxQuat(q.x, q.y, -q.z, q.w);
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
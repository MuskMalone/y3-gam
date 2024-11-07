#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Core/Entity.h>
namespace IGE {
	namespace Physics {
		inline glm::vec3 ToGLMVec3(physx::PxVec3 const& v) {
			return glm::vec3{v.x, v.y, v.z };
		}
		inline physx::PxVec3 ToPxVec3(glm::vec3 const& v){
			return physx::PxVec3(v.x, v.y, v.z);
		}

		inline physx::PxVec3 ToPxEuler(const physx::PxQuat& quat) {
			// Convert PhysX quaternion to GLM quaternion
			glm::quat glmQuat(quat.w, quat.x, quat.y, quat.z);

			// Convert GLM quaternion to Euler angles
			glm::vec3 euler = glm::degrees(glm::eulerAngles(glmQuat));

			// Return Euler angles with inverted Z-axis for PhysX
			return physx::PxVec3(euler.x, euler.y, euler.z);  // Invert Z to match PhysX coordinate system
		}

		inline glm::quat ToGLMQuat(const physx::PxQuat& q) {
			// for some reason glm initializes quats as wxyz
			return glm::quat(q.w, q.x, q.y, q.z);
		}
		// Convert PxQuat to Euler angles (roll, pitch, yaw) using GLM

		// Convert Euler angles (roll, pitch, yaw) to PxQuat using GLM
		inline physx::PxQuat ToPxQuat(const physx::PxVec3& euler) {
			// Create GLM Euler angles with inverted Z-axis
			glm::vec3 glmEuler(euler.x, euler.y, euler.z);

			// Convert Euler angles to GLM quaternion
			glm::quat glmQuat = glm::quat(glm::radians(glmEuler));

			// Convert GLM quaternion to PhysX quaternion
			return physx::PxQuat(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
		}


		inline physx::PxQuat ToPxQuat(const glm::quat& q) {
			return physx::PxQuat(q.x, q.y, q.z, q.w);
		}

		inline glm::quat MultiplyQuaternionRotation(const glm::quat& quat, float magnitude) {
			// Extract the angle and axis from the quaternion
			float angle = glm::angle(quat);  // Gets the angle of rotation in radians
			glm::vec3 axis = glm::axis(quat); // Gets the axis of rotation (normalized)

			// Halve the angle
			float halfAngle = angle * magnitude;

			// Create a new quaternion with the halved angle
			glm::quat halvedQuat = glm::angleAxis(halfAngle, axis);
			return halvedQuat;
		}

		inline physx::PxQuat MultiplyQuaternionRotation(const physx::PxQuat& q, float magnitude) {
			glm::quat quat = ToGLMQuat(q);
			glm::quat halvedQuat = MultiplyQuaternionRotation(quat, magnitude);
			return ToPxQuat(halvedQuat);
		}

		template <typename T>
		inline T ToPhysicsUnits(T const& v) {
			return v * .5f;
		}
		template <typename T>
		inline T ToGraphicUnits(T const& v) {
			return v * 2.f;
		}

		template <>
		inline glm::quat ToPhysicsUnits(glm::quat const& v) {
			return MultiplyQuaternionRotation(v, .5f);
		}

		template <>
		inline glm::quat ToGraphicUnits(glm::quat const& v) {
			return MultiplyQuaternionRotation(v, 2.f);
		}

		template <>
		inline physx::PxQuat ToPhysicsUnits(physx::PxQuat const& v) {
			return MultiplyQuaternionRotation(v, .5f);
		}

		template <>
		inline physx::PxQuat ToGraphicUnits(physx::PxQuat const& v) {
			return MultiplyQuaternionRotation(v, 2.f);
		}
		
	}
}
/*!*********************************************************************
\file   RigidBody.h
\author 
\date   6-October-2024
\brief  Definition of RigidBody component used by the physics system
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <PxPhysicsAPI.h>

namespace Component {

	enum class RigidBodyVars {
		MASS,
		STATIC_FRICTION,
		DYNAMIC_FRICTION,
		RESTITUTION,
		GRAVITY_FACTOR,
		VELOCITY,
		ANGULAR_VELOCITY,
		MOTION,
		LOCK,
		LINEAR_DAMPING,
		ANGULAR_DAMPING, 
		FORCE, //will take the velocity of RigidBody as the target, then apply a force to achieve that target speed
		HAS_JOINT,
		JOINT_ENTITY,
		JOINT_PROPS,
		JOINT_TYPE
	};

	struct RigidBody {
		enum class MotionType {
			DYNAMIC = 0,
			KINEMATIC
		};

		enum class Axis : int{
			X = 1 << 0, Y = 1 << 1, Z = 1 << 2
		};
		enum class JointType {
			REVOLUTE,
			SPHERICAL,
			PRISMATIC,
			DISTANCE
		};
		struct JointConfig {
			JointType jointType = JointType::REVOLUTE;

			float breakForce = FLT_MAX; // The force at which the joint will break.
			float breakTorque = FLT_MAX; // The torque at which the joint will break.

			// Spring/drive properties (for joints that support motorization or spring behavior):
			float stiffness = 0.f;      // How stiff the joint is.
			float damping = 0.f;        // How quickly it dissipates energy.

			// Revolute-specific limits (angles in radians)
			float lowerAngle = 0.f;     // Minimum angular limit (for revolute joints).
			float upperAngle = 0.f;     // Maximum angular limit (for revolute joints).

			// Spherical-specific limits (angles in radians)
			float yLimit = 0.01f;     // Maximum swing limit (for spherical joints).
			float zLimit = 0.01f;     // Maximum twist limit (for spherical joints).

			// Prismatic/Distance-specific limits (linear distance units)
			float lowerLimit = 0.f;     // Minimum translation/distance.
			float upperLimit = 0.f;     // Maximum translation/distance.

			// Motor/drive properties (if applicable)
			bool motorEnabled = false;
			float motorTargetVelocity = 0.f;
			float motorForceLimit = 0.f;
		};

		inline void Clear() noexcept {
			// Idk what are good default values
			velocity = physx::PxVec3();
			angularVelocity = physx::PxVec3();
			staticFriction = 1.f;
			restitution = 1.f;
			gravityFactor = 1.f;
			motionType = MotionType::KINEMATIC;
		}

		float mass{ 1.f };
		physx::PxVec3 velocity{0, 0, 0};
		physx::PxVec3 angularVelocity{0, 0, 0};
		physx::PxVec3 force{ 0, 0, 0 }; //not meant to be saved or recorded. passing as a parameter to move rb
		float staticFriction;
		float dynamicFriction;
		float restitution;
		float gravityFactor{ 0.f };
		float linearDamping{ 0.98f };
		MotionType motionType{ MotionType::KINEMATIC }; //static, dynamic, kinematic
		//locks the axis the rb can move in
		int axisLock{0};
		//locks the axis the rb can rotate around
		int angularAxisLock{0};

		bool hasJoint{ false };          // Does this body have a joint associated with it?
		unsigned entityLinked{ static_cast<unsigned>(-1) }; // For linking to another entity if jointed.

		// New joint configuration. Only used if hasJoint == true.
		JointConfig jointConfig;
		physx::PxVec3 jointOffset{0,0,0};
		physx::PxVec3 entityLinkedJointOffset{0,0,0};

		void SetImpulse(glm::vec3 const& impulse);
		void SetForce(glm::vec3 const& force);
		void SetPosition(glm::vec3 const& pos);
		void SetAxisLock(int lock);
		void RemoveAxisLock(int lock);
		bool IsAxisLocked(int axis);
		void SetAngleAxisLock(int lock);
		void RemoveAngleAxisLock(int lock);
		bool IsAngleAxisLocked(int axis);
		void* bodyID;
		void* jointID;

	};
} // namespace Component
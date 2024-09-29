#pragma once

namespace Component {
	enum class RigidBodyVars {
		MASS,
		STATIC_FRICTION, 
		DYNAMIC_FRICTION,
		RESTITUTION,
		GRAVITY_FACTOR,
		VELOCITY,
		ANGULAR_VELOCITY,
		MOTION
	};
	
	struct RigidBody {
		enum class MotionType {
			STATIC,
			KINEMATIC,
			DYNAMIC
		};
		inline void Clear() noexcept { 
			// Idk what are good default values
			velocity = physx::PxVec3();
			angularVelocity = physx::PxVec3();
			staticFriction = 1.f;
			restitution = 1.f;
			gravityFactor = 1.f;
			motionType = MotionType::STATIC;
		}

		float mass{1.f};
		physx::PxVec3 velocity{0,0,0};
		physx::PxVec3 angularVelocity{0,0,0};
		float staticFriction;
		float dynamicFriction;
		float restitution;
		float gravityFactor{0.f};
		float linearDamping{ 0.5f };
		MotionType motionType{ MotionType::STATIC }; //static, dynamic, kinematic
		void* bodyID;

	};
} // namespace Component
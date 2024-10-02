#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vector.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/MotionType.h>

namespace Component {
	enum class RigidBodyVars {
		FRICTION, 
		RESTITUTION,
		GRAVITY_FACTOR,
		VELOCITY,
		ANGULAR_VELOCITY,
		MOTION
	};

	struct RigidBody {

		inline void Clear() noexcept { 
			// Idk what are good default values
			velocity = JPH::Vec3();
			angularVelocity = JPH::Vec3();
			friction = 1.f;
			restitution = 1.f;
			gravityFactor = 1.f;
			motionType = JPH::EMotionType::Static;
		}

		//float mass // mass cannot be set on the fly like how other variables can 
		JPH::Vec3 velocity;
		JPH::Vec3 angularVelocity;
		float friction;
		float restitution;
		float gravityFactor{1.f};
		JPH::BodyID bodyID;
		JPH::EMotionType motionType{ JPH::EMotionType::Static }; //static, dynamic, kinematic
	};
} // namespace Component
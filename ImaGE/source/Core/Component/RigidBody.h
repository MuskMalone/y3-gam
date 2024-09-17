#pragma once

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
		//float mass //mass cannot be set on the fly like how other variables can 
		JPH::Vec3 velocity;
		JPH::Vec3 angularVelocity;
		float friction;
		float restitution;
		float gravityFactor{1.f};
		JPH::BodyID bodyID;
		JPH::EMotionType motionType{ JPH::EMotionType::Static }; //static, dynamic, kinematic
	};
}

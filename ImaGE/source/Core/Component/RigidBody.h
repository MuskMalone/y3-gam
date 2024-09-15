#pragma once
#include <pch.h>

namespace Component {
	struct RigidBody {
		bool useGravity{true};
		//float mass //mass cannot be set on the fly like how other variables can 
		float friction;
		float restitution;
		float gravityFactor{1.f};
		JPH::Vec3 velocity;
		JPH::Vec3 angularVelocity;
		JPH::BodyID bodyID;
		JPH::EMotionType motionType{JPH::EMotionType::Static}; //static, dynamic, kinematic
	};
}
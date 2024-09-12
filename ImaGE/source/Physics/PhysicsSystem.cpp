#include <pch.h>
#include "Physics/PhysicsSystem.h"

namespace IGE {
	namespace Physics {
		PhysicsSystem::PhysicsSystem() :
			mTempAllocator{ 10 * 1024 * 1024 },
			mJobSystem{ cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1 }
		{}
		void PhysicsSystem::Init(){
			JPH::RegisterDefaultAllocator();
			JPH::Trace = TraceImpl;
			JPH::Factory::sInstance = new JPH::Factory();
			JPH::RegisterTypes();
			mPhysicsSystem.Init(
				cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
				mBroadPhaseLayerInterface, mObjectVsBroadphaseLayerFilter, mObjectVsObjectLayerFilter
			);
			mPhysicsSystem.SetBodyActivationListener(&mBodyActivationListener);
			mPhysicsSystem.SetContactListener(&mContactListener);
		}

		void PhysicsSystem::Update(float dt){
			
		}

		void PhysicsSystem::OnEntityAdd(){
			mPhysicsSystem.OptimizeBroadPhase();
			//left empty for now
		}

		void PhysicsSystem::Debug(float dt){
		}

		void PhysicsSystem::Release(){
			delete JPH::Factory::sInstance;
		}

	}
}
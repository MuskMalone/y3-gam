#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include "Core/Component/Components.h"
namespace IGE {
	namespace Physics {

		std::shared_ptr<IGE::Physics::PhysicsSystem> PhysicsSystem::_mSelf;
		std::mutex PhysicsSystem::_mMutex;
		std::shared_ptr<IGE::Physics::PhysicsSystem> PhysicsSystem::GetInstance()
		{
			std::lock_guard<std::mutex> lock(_mMutex);
			if (_mSelf == nullptr) {
				_mSelf = std::make_shared<IGE::Physics::PhysicsSystem>();
			}
			return _mSelf;
		}
		PhysicsSystem::PhysicsSystem() :
			mTempAllocator{ 10 * 1024 * 1024 },
			mJobSystem{ cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1 }
		{
		
		}
		void PhysicsSystem::InitAllocator()
		{
			JPH::RegisterDefaultAllocator();
			JPH::Trace = TraceImpl;
			JPH::Factory::sInstance = new JPH::Factory();
			JPH::RegisterTypes();
		}
		void PhysicsSystem::Init(){
			

			mPhysicsSystem.Init(
				cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
				mBroadPhaseLayerInterface, mObjectVsBroadphaseLayerFilter, mObjectVsObjectLayerFilter
			);
			mPhysicsSystem.SetBodyActivationListener(&mBodyActivationListener);
			mPhysicsSystem.SetContactListener(&mContactListener);
			mPhysicsSystem.SetGravity(JPH::Vec3(0.f, -9.81f, 0.f));
		}

		void PhysicsSystem::Update(float dt){

			mPhysicsSystem.Update(gDeltaTime, 1, &mTempAllocator, &mJobSystem);

		}

		void PhysicsSystem::OnEntityAdd(){
			//Component::Collider collider {
			//	.scale = JPH::Vec3(obj->transform.scale.x, obj->transform.scale.y, obj->transform.scale.z),
			//		.position = JPH::Vec3(obj->transform.pos.x, obj->transform.pos.y, obj->transform.pos.z),
			//		.rotation = JPH::Quat::sEulerAngles(JPH::Vec3(obj->transform.rot.x, obj->transform.rot.x, obj->transform.rot.x))
			//};

			//JPH::BodyCreationSettings bodySettings(new SphereShape(collider.scale.GetX()), collider.position, collider.rotation,
			//	JPH::EMotionType::Dynamic, Layers::MOVING);
			////bodySettings.mMassPropertiesOverride = collider.shape->GetMassProperties(5.f); // Set mass 
			//collider.bodyID = mPhysicsSystem.GetBodyInterface().CreateAndAddBody(bodySettings, EActivation::Activate); // Activ
			//collider.type = mPhysicsSystem.GetBodyInterface().GetShape(collider.bodyID)->GetSubType();
			//mPhysicsSystem.OptimizeBroadPhase();
			//left empty for now
		}

		void PhysicsSystem::Debug(float dt){
		}

		void PhysicsSystem::Release(){
			delete JPH::Factory::sInstance;
		}

	}
}
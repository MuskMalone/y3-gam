#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include "Core/Component/Components.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
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

		void PhysicsSystem::AddRigidBody(ECS::Entity entity) {


			Component::RigidBody rigidbody{};
			auto& bodyinterface = mPhysicsSystem.GetBodyInterface();
			if (!entity.HasComponent<Component::Collider>()) {
				Component::Transform const& transform = entity.GetComponent<Component::Transform>();
				auto* shape = new BoxShape(ToJPHVec3(transform.worldScale)); // this is an arbitrary shape, since the collider should be updated in another component
				JPH::BodyCreationSettings bodySettings(shape, ToJPHVec3(transform.worldPos), Quat::sEulerAngles(ToJPHVec3(transform.worldRot)),
					rigidbody.motionType, Layers::MOVING);
				//bodySettings.mMassPropertiesOverride = collider.shape->GetMassProperties(5.f); // Set mass 
				
				rigidbody.bodyID = bodyinterface.CreateAndAddBody(bodySettings, EActivation::DontActivate); // Active
				JPH::Body* body = mPhysicsSystem.GetBodyLockInterface().TryGetBody(rigidbody.bodyID);
			}
			//set 
			bodyinterface.SetFriction(rigidbody.bodyID, rigidbody.friction);
			bodyinterface.SetRestitution(rigidbody.bodyID, rigidbody.restitution);
			bodyinterface.SetGravityFactor(rigidbody.bodyID, rigidbody.gravityFactor);
			bodyinterface.SetAngularVelocity(rigidbody.bodyID, rigidbody.angularVelocity);
			entity.EmplaceOrReplaceComponent<Component::RigidBody>(rigidbody);
			mPhysicsSystem.OptimizeBroadPhase();
			
		}

		void PhysicsSystem::ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var)
		{
		}

		void PhysicsSystem::AddCollider(ECS::Entity entity)
		{
			//empty. rigidbody is the collider 

		}

		void PhysicsSystem::ChangeColliderShape(ECS::Entity entity)
		{
		}

		void PhysicsSystem::ChangeColliderVar(ECS::Entity entity, Component::ColliderVars var)
		{
		}
		
		void PhysicsSystem::Debug(float dt){
		}

		void PhysicsSystem::Release(){
			delete JPH::Factory::sInstance;
		}

	}
}
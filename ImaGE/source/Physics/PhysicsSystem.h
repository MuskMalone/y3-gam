#pragma once
#include <pch.h>
#include <Core/Component/Components.h>
#include <Physics/Physics.h>
#include <Core/Entity.h>
namespace IGE {
	namespace Physics {
		const float gDeltaTime = 1.f / 60.f;

		class PhysicsSystem {
		public:
			static std::shared_ptr<IGE::Physics::PhysicsSystem> GetInstance();
			PhysicsSystem();
			static void InitAllocator();
			void Init();
			void Update(float dt);

			void AddRigidBody(ECS::Entity entity); // should const ref this when the functions r consted
			void ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var);

			void AddCollider(ECS::Entity entity);
			void ChangeColliderShape(ECS::Entity entity);
			void ChangeColliderVar(ECS::Entity entity, Component::ColliderVars var);

			void Debug(float dt); // not sure if this function will be needed;
			void Release();
		private:
			const uint32_t cMaxBodies = 65536;
			const uint32_t cNumBodyMutexes = 0;
			const uint32_t cMaxBodyPairs = 65536;
			const uint32_t cMaxContactConstraints = 10240;

		private:
			JPH::TempAllocatorImpl mTempAllocator;
			JPH::JobSystemThreadPool mJobSystem;
			BPLayerInterfaceImpl mBroadPhaseLayerInterface;
			ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadphaseLayerFilter;
			ObjectLayerPairFilterImpl mObjectVsObjectLayerFilter;

			JPH::PhysicsSystem mPhysicsSystem;
			MyBodyActivationListener mBodyActivationListener;
			MyContactListener mContactListener;

			static std::shared_ptr<IGE::Physics::PhysicsSystem> _mSelf;
			static std::mutex _mMutex;
			PhysicsSystem(PhysicsSystem& other) = delete;
			void operator=(const PhysicsSystem&) = delete;
		};
	}
}
#pragma once
#include <Core/Components/Components.h>
#include <Core/Entity.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Events/EventCallback.h>
namespace IGE {
	namespace Physics {
		const float gDeltaTime = 1.f / 60.f;

		class PhysicsSystem : public Systems::System {
		public:
			static std::shared_ptr<IGE::Physics::PhysicsSystem> GetInstance();
			PhysicsSystem();
			~PhysicsSystem();
			void Update() override;

			// temporarily using this to deserialize a copy of the rb from file
			Component::RigidBody& AddRigidBody(ECS::Entity entity, Component::RigidBody rb = {}); // should const ref this when the functions r consted
			void ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var);

			// temporarily using this to deserialize a copy of the collider from file
			Component::BoxCollider& AddBoxCollider(ECS::Entity entity, Component::BoxCollider collider = {});
			Component::SphereCollider& AddSphereCollider(ECS::Entity entity, Component::SphereCollider collider = {});
			Component::CapsuleCollider& AddCapsuleCollider(ECS::Entity entity, Component::CapsuleCollider collider = {});
			void ChangeBoxColliderVar(ECS::Entity entity);
			void ChangeSphereColliderVar(ECS::Entity entity);
			void ChangeCapsuleColliderVar(ECS::Entity entity);
			void Debug(float dt); // to be called within rendersystems geom pass
		//private:
		//	const uint32_t cMaxBodies = 65536;
		//	const uint32_t cNumBodyMutexes = 0;
		//	const uint32_t cMaxBodyPairs = 65536;
		//	const uint32_t cMaxContactConstraints = 10240;

		private:
			//JPH::TempAllocatorImpl mTempAllocator;
			//JPH::JobSystemThreadPool mJobSystem;
			//BPLayerInterfaceImpl mBroadPhaseLayerInterface;
			//ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadphaseLayerFilter;
			//ObjectLayerPairFilterImpl mObjectVsObjectLayerFilter;

			//JPH::PhysicsSystem mPhysicsSystem;
			//MyBodyActivationListener mBodyActivationListener;
			//MyContactListener mContactListener;
			physx::PxDefaultAllocator      mAllocator;
			physx::PxDefaultErrorCallback  mErrorCallback;
			physx::PxFoundation* mFoundation = nullptr;
			physx::PxPvd* mPvd = nullptr;
			physx::PxPhysics* mPhysics = nullptr;
			physx::PxMaterial* mMaterial = nullptr;
			physx::PxScene* mScene = nullptr;
			// just the pointers returned from createdynamic but in void* form
			// map is a way to get around the pesky casting
			std::unordered_map<void*, physx::PxRigidDynamic*> mRigidBodyIDs; 
			static std::shared_ptr<IGE::Physics::PhysicsSystem> _mSelf;
			static std::mutex _mMutex;
			PhysicsSystem(PhysicsSystem& other) = delete;
			void operator=(const PhysicsSystem&) = delete;

		private:
			template <typename _component_type>
			physx::PxShape* GetShapePtr(_component_type const& collider);
			template <typename _physx_type, typename _collider_component>
			void RemoveCollider(ECS::Entity entity, physx::PxGeometryType::Enum typeenum);
			void RemoveRigidBody(ECS::Entity entity);
			EVENT_CALLBACK_DECL(HandleRemoveComponent);
			EVENT_CALLBACK_DECL(HandleRemoveEntity);
			template <typename _physx_type, typename _collider_component>
			void AddShape(physx::PxRigidDynamic* rb, ECS::Entity const& entity, _collider_component& collider);
			template <typename _physx_type, typename _collider_component>
			void AddNewCollider(physx::PxRigidDynamic*& rb, ECS::Entity const& entity, _collider_component& collider);
			template<typename _physx_type, typename _collider_component>
			_collider_component& AddCollider(ECS::Entity entity, _collider_component collider);
		};
	}
}
template <>
inline void Systems::SystemManager::RegisterSystem<IGE::Physics::PhysicsSystem>(const char* name) {
	SystemPtr sys{ IGE::Physics::PhysicsSystem::GetInstance() };
	mNameToSystem.emplace(typeid(IGE::Physics::PhysicsSystem).name(), sys);
	mSystems.emplace_back(std::move(sys));
}

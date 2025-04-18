#pragma once
#include <Core/Components/Components.h>
#include <Core/Entity.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Events/EventCallback.h>
#include <Physics/PhysicsEventManager.h>
namespace IGE {
	namespace Physics {
		const float gDeltaTime = 1.f / 60.f;
		struct RaycastHit {
			ECS::Entity entity{};
			glm::vec3 position;
			glm::vec3 normal;
			float distance;
		};
		struct RaycastHitInfo {
			glm::vec3 position{std::numeric_limits<float>::max()};
			glm::vec3 normal{};
			float distance{ std::numeric_limits<float>::max() };
		};
		struct GeneralRay {
			glm::vec3 start;
			glm::vec3 end;
		};
		class PhysicsSystem : public Systems::System {
		private:
			struct RayCastResult {
				glm::vec3 origin;
				glm::vec3 end;
				RaycastHit hit;
				bool detected;
			};

		public:
			static std::shared_ptr<IGE::Physics::PhysicsSystem> GetInstance();
			PhysicsSystem();
			void UpdatePhysicsToTransform(ECS::Entity e, bool updatePosition = true);
			~PhysicsSystem();

			void Update() override;
			void PausedUpdate() override;

			void LoadJoints();

			// temporarily using this to deserialize a copy of the rb from file
			Component::RigidBody& AddRigidBody(ECS::Entity entity, Component::RigidBody rb = {}); // should const ref this when the functions r consted
			void ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var);

			// temporarily using this to deserialize a copy of the collider from file
			Component::BoxCollider& AddBoxCollider(ECS::Entity entity, bool newCollider, Component::BoxCollider collider = {});
			Component::SphereCollider& AddSphereCollider(ECS::Entity entity, bool newCollider, Component::SphereCollider collider = {});
			Component::CapsuleCollider& AddCapsuleCollider(ECS::Entity entity, bool newCollider, Component::CapsuleCollider collider = {});
			void ChangeBoxColliderVar(ECS::Entity entity);
			void ChangeSphereColliderVar(ECS::Entity entity);
			void ChangeCapsuleColliderVar(ECS::Entity entity);
			void Debug(); // to be called within rendersystems geom pass
			void ClearSystem(); //clears all the rigidbodies. 
			std::unordered_map<void*, physx::PxRigidDynamic*> const& GetRigidBodyIDs() const { return mRigidBodyIDs; }
			
			//returns the first object hit
			bool RayCastSingular(
				glm::vec3 const& origin, glm::vec3 const& end,
				RaycastHit& result
			);

			//casts a ray(direction * magnitude) from the physics global position ( must have a rigidbody or collider)
			bool RayCastFromEntity(
				ECS::Entity entity, glm::vec3 const& direction, float magnitude,
				RaycastHit& result
			);
			//casts a ray from origin to end, will ignore the entity provided
			bool RayCastFromEntity(
				ECS::Entity entity, glm::vec3 const& origin, glm::vec3 const& end,
				RaycastHit& result
			);
			bool OnTriggerPersists(ECS::Entity trigger, ECS::Entity other);
			bool OnTriggerEnter(ECS::Entity trigger, ECS::Entity other);
			bool OnTriggerExit(ECS::Entity trigger, ECS::Entity other);
			std::vector<physx::PxContactPairPoint> GetContactPoints(ECS::Entity entity1, ECS::Entity entity2);

			float GetShortestDistance(ECS::Entity e1, ECS::Entity e2);

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
			std::unordered_map<void*, ECS::Entity> mRigidBodyToEntity;
			std::unordered_map<void*, std::set<unsigned>> mJointMap; // map of entityB joints connected to other joints to track for removal

			std::unordered_map<void*, std::unordered_map<void*, int>> mOnTriggerPairs;
			std::unordered_map<void*, std::unordered_map<void*, std::vector<physx::PxContactPairPoint>>> mOnContactPairs;
			static std::shared_ptr<IGE::Physics::PhysicsSystem> _mSelf;
			static std::mutex _mMutex;
			PhysicsSystem(PhysicsSystem& other) = delete;
			void operator=(const PhysicsSystem&) = delete;
			static std::unordered_set<physx::PxRigidDynamic*> mInactiveActors;
			std::vector<RayCastResult> mRays;
			std::vector<GeneralRay> mGeneralRays;
			bool mDrawDebug{ false };

		public:
			PhysicsEventManager* mEventManager;
		private:
			template <typename _component_type>
			physx::PxShape* GetShapePtr(_component_type const& collider);
			template <typename _physx_type, typename _collider_component>
			void RemoveCollider(ECS::Entity entity, physx::PxGeometryType::Enum typeenum);
			void RemoveRigidBody(ECS::Entity entity);
			EVENT_CALLBACK_DECL(HandleRemoveComponent);
			EVENT_CALLBACK_DECL(HandleRemoveEntity);
			template <typename _physx_type, typename _collider_component>
			void AddShape(physx::PxRigidDynamic* rb, ECS::Entity const& entity, _collider_component& collider, bool newCollider);
			template <typename _physx_type, typename _collider_component>
			void AddNewCollider(physx::PxRigidDynamic*& rb, ECS::Entity const& entity, _collider_component& collider, bool newCollider);
			template<typename _physx_type, typename _collider_component>
			_collider_component& AddCollider(ECS::Entity entity, _collider_component collider, bool newCollider);
			template<typename _physx_type, typename _collider_component>
			physx::PxShape* CreateShape(_physx_type const& geom, _collider_component const& collider, ECS::Entity entity);
			void RegisterRB(void* bodyID, physx::PxRigidDynamic* rbptr, ECS::Entity const& entity) noexcept;
			void RemoveRB(void* bodyID) noexcept;
			physx::PxRigidDynamic* GetRBIter(ECS::Entity entity);
			void SetEntityActive(ECS::Entity e, physx::PxRigidDynamic* pxrb, bool kinematic);
			void CreateJoint(ECS::Entity entity, Component::RigidBody& rb, physx::PxRigidDynamic* rbptr, physx::PxRigidDynamic* otherRbptr);
			void UpdateJointConfig(ECS::Entity entity, Component::RigidBody& rb);
			void RemoveJoint(ECS::Entity entity, Component::RigidBody& rb);

		private:
			//for testing purposes only
			PHYSICS_EVENT_LISTENER_DECL(OnContactSampleListener)
			PHYSICS_EVENT_LISTENER_DECL(OnTriggerSampleListener)
		private:
			friend IGE::Physics::PhysicsEventManager;
			friend Component::RigidBody;
			friend Component::BoxCollider;
			friend Component::SphereCollider;
			friend Component::CapsuleCollider;
		};
	}

	physx::PxFilterFlags LayerFilterShaderWrapper(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
}
template <>
inline void Systems::SystemManager::RegisterSystem<IGE::Physics::PhysicsSystem>(const char* name) {
	SystemPtr sys{ IGE::Physics::PhysicsSystem::GetInstance() };
	mNameToSystem.emplace(typeid(IGE::Physics::PhysicsSystem).name(), sys);
	mSystems.emplace_back(std::move(sys));
}

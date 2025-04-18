#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include <Core/Components/Components.h>
#include <Core/LayerManager/LayerManager.h>
#include "Core/EntityManager.h"
#include "Events/EventManager.h"
#include "Core/Entity.h"
#include "Physics/PhysicsHelpers.h"
#include "Physics/PhysicsEventData.h"
#include "Graphics/Renderer.h"
#include "Input/InputManager.h"
#include "Asset/AssetManager.h"
namespace IGE {
	namespace Physics {
		void SetColliderAsSensor(physx::PxShape* shapeptr, bool sensor);
		template <typename... _component>
		bool HasAnyComponent(ECS::Entity entity) {
			return (... || entity.HasComponent<_component>());
		}
		std::unordered_set<physx::PxRigidDynamic*> PhysicsSystem::mInactiveActors{};
		
		const float gGravity{ -9.81f };
		const float gTimeStep{ 1.f / 60.f };
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
		PhysicsSystem::PhysicsSystem() : Systems::System{ "PhysicsSystem" },
			mAllocator{}, mErrorCallback{},
			mFoundation{ PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback) },
			mPvd{ PxCreatePvd(*mFoundation) },
			mPhysics{ PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true, mPvd) },
			mMaterial{ mPhysics->createMaterial(0.5f, 0.5f, 0.6f) },
			mRigidBodyIDs{}, mRigidBodyToEntity{}, mEventManager{ new PhysicsEventManager{mRigidBodyIDs, mRigidBodyToEntity, mOnTriggerPairs, mOnContactPairs} }
			//mTempAllocator{ 10 * 1024 * 1024 },
			//mJobSystem{ cMaxPhysicsJobs, cMaxPhysicsBarriers, static_cast<int>(thread::hardware_concurrency() - 1) }
		{
			mMaterial->setFrictionCombineMode(physx::PxCombineMode::eMULTIPLY);

			SUBSCRIBE_CLASS_FUNC(Events::RemoveComponentEvent, &PhysicsSystem::HandleRemoveComponent, this);
			SUBSCRIBE_CLASS_FUNC(Events::RemoveEntityEvent, &PhysicsSystem::HandleRemoveEntity, this);

			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			// Create the scene (simulation world)
			physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f); // Gravity pointing downward

			// Use default CPU dispatcher
			physx::PxDefaultCpuDispatcher* dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = dispatcher;

			sceneDesc.filterShader = LayerFilterShaderWrapper;
			mScene = mPhysics->createScene(sceneDesc);

			// register callbacks for events
			mScene->setSimulationEventCallback(mEventManager);

			mEventManager->AddListener(PHYSICS_METHOD_LISTENER(PhysicsEventID::CONTACT, PhysicsSystem::OnContactSampleListener));
			mEventManager->AddListener(PHYSICS_METHOD_LISTENER(PhysicsEventID::TRIGGER, PhysicsSystem::OnTriggerSampleListener));
		}

		void PhysicsSystem::SetEntityActive(ECS::Entity e, physx::PxRigidDynamic* pxrb, bool isKinematic) {
			if (!e.IsActive()) {
				// If not already disabled, set the flag.
				if (!pxrb->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_SIMULATION)) {
					if (isKinematic)
						pxrb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false); // doesnt work for kinematic colliders
					pxrb->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
				}
				// Optionally, you can skip updating transforms since the actor won�t move.
				return;
			}
			else {
				// If the entity is active, ensure simulation is enabled.
				if (pxrb->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_SIMULATION)) {
					pxrb->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
					if (isKinematic)
						pxrb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
				}
			}
		}
		void PhysicsSystem::UpdatePhysicsToTransform(ECS::Entity e, bool updatePos) {
			auto& xfm{ e.GetComponent<Component::Transform>() };
			if (HasAnyComponent<Component::RigidBody, Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(e)) {
				physx::PxRigidDynamic* pxrb{};
				bool isKinematic{ false };
				auto rbiter{ mRigidBodyIDs.find(nullptr) };
				if (e.HasComponent<Component::RigidBody>())
					rbiter = mRigidBodyIDs.find(e.GetComponent<Component::RigidBody>().bodyID), isKinematic = (bool)e.GetComponent<Component::RigidBody>().motionType;
				else if (e.HasComponent<Component::BoxCollider>())
					rbiter = mRigidBodyIDs.find(e.GetComponent<Component::BoxCollider>().bodyID), isKinematic = false;
				else if (e.HasComponent<Component::SphereCollider>())
					rbiter = mRigidBodyIDs.find(e.GetComponent<Component::SphereCollider>().bodyID), isKinematic = false;
				else if (e.HasComponent<Component::CapsuleCollider>())
					rbiter = mRigidBodyIDs.find(e.GetComponent<Component::CapsuleCollider>().bodyID), isKinematic = false;

				if (rbiter != mRigidBodyIDs.end()) {
					pxrb = rbiter->second;

					//if (!e.IsActive()) {
					//	if (mInactiveActors.find(pxrb) == mInactiveActors.end()) {
					//		mScene->removeActor(*pxrb);
					//		mInactiveActors.insert(pxrb);
					//	}
					//	return;
					//}
					//else {
					//	if (mInactiveActors.find(pxrb) != mInactiveActors.end()) {
					//		mScene->addActor(*pxrb);
					//		mInactiveActors.erase(pxrb);
					//	}
					//}
					// Instead of removing/adding actors, disable simulation for inactive entities.
					SetEntityActive(e, pxrb, isKinematic);
				}

				else throw std::runtime_error{ std::string("there is no rigidbody ") };


				if (updatePos)
				{
					//getting from graphics
					if (pxrb->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
						pxrb->setKinematicTarget(physx::PxTransform{ ToPxVec3(xfm.worldPos), ToPxQuat(xfm.worldRot) });
					else
						pxrb->setGlobalPose(physx::PxTransform{ ToPxVec3(xfm.worldPos), ToPxQuat(xfm.worldRot) });
				}
			}

			//JPH::BodyInterface& bodyInterface { mPhysicsSystem.GetBodyInterface() };
			//xfm.worldPos = ToGLMVec3(bodyInterface.GetPosition(rb.bodyID));
			//xfm.worldPos = ToGLMVec3(bodyInterface.(rb.bodyID));

		}

		void PhysicsSystem::Update() {
			// Static accumulator to keep track of elapsed time.
			// (You may want to move this to a member variable if needed.)
			static float physicsAccumulator = 0.f;
			physicsAccumulator += Performance::FrameRateController::GetInstance().GetDeltaTime();

			//set the entities to active/inactive
			auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Transform>() };
			for (auto entity : rbsystem) {
				UpdatePhysicsToTransform(entity, false);
			}

			mOnTriggerPairs.clear();
			mOnContactPairs.clear();
			// Run the physics simulation in fixed time steps.
			while (physicsAccumulator >= gTimeStep) {
				// Clear any previous frame data (if needed for this physics step).

				// Simulate one fixed time step (e.g., 1/60 second).
				mScene->simulate(gTimeStep);
				mScene->fetchResults(true);

				// Retrieve only the rigid bodies (that are the only ones that can move)
				auto rbsystem = ECS::EntityManager::GetInstance()
					.GetAllEntitiesWithComponents<Component::RigidBody, Component::Transform>();
				for (auto entity : rbsystem) {
					ECS::Entity e{ entity };
					if (e.HasComponent<Component::Animation>() && e.GetComponent<Component::Animation>().GetCurrentAnimation().second) {
							//runs if the animation is currently running
						UpdatePhysicsToTransform(e);
						continue;

					}
					auto& xfm = rbsystem.get<Component::Transform>(entity);
					auto& rb = rbsystem.get<Component::RigidBody>(entity);

					auto rbiter = mRigidBodyIDs.find(rb.bodyID);
					if (rbiter != mRigidBodyIDs.end()) {
						physx::PxRigidDynamic* pxrigidbody = mRigidBodyIDs.at(rb.bodyID);

						// Only add gravity in this fixed timestep update.
						if (rb.motionType == Component::RigidBody::MotionType::DYNAMIC) {
							float gravForce = gGravity * rb.gravityFactor * rb.mass;
							// Apply gravity force. (If needed, you could also multiply by gTimeStep here,
							// but if your physics simulation is set up to expect a per-timestep force, then this is fine.)
							pxrigidbody->addForce(ToPxVec3(glm::vec3(0.f, gravForce, 0.f)));
						}

						// Update the transform based on the physics simulation result.
						auto pose = pxrigidbody->getGlobalPose();
						xfm.worldPos = ToGLMVec3(pose.p);
						xfm.worldRot = ToGLMQuat(pose.q);
						xfm.ComputeWorldMtx();
						xfm.modified = true;

						// Update the rigid body's velocities.
						rb.velocity = pxrigidbody->getLinearVelocity();
						rb.angularVelocity = pxrigidbody->getAngularVelocity();
					}
				}

				// Decrement the accumulator by the fixed timestep.
				physicsAccumulator -= gTimeStep;
			}

		}


		void PhysicsSystem::PausedUpdate() {
			auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Transform>() };
			for (auto entity : rbsystem) {
				UpdatePhysicsToTransform(entity);
			}
		}
		void PhysicsSystem::LoadJoints() {
			auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::RigidBody>() };
			for (auto e : rbsystem) {
				ECS::Entity entity{ e };
				auto& rigidbody{ entity.GetComponent<Component::RigidBody>() };
				ChangeRigidBodyVar(entity, Component::RigidBodyVars::HAS_JOINT);
				ChangeRigidBodyVar(entity, Component::RigidBodyVars::JOINT_ENTITY);
				ChangeRigidBodyVar(entity, Component::RigidBodyVars::JOINT_PROPS);
			}
		}
		Component::RigidBody& PhysicsSystem::AddRigidBody(ECS::Entity entity, Component::RigidBody rigidbody) {
			physx::PxRigidDynamic* rb { }; // Mass = 10.0f
			//if (entity.HasComponent<Component::BoxCollider>() ||
			//	entity.HasComponent<Component::SphereCollider>() ||
			//	entity.HasComponent<Component::CapsuleCollider>()) {
			if (HasAnyComponent<Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(entity)){
				auto rbiter{ mRigidBodyIDs.find(nullptr) };
				if (entity.HasComponent<Component::BoxCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::BoxCollider>().bodyID);
				else if (entity.HasComponent<Component::SphereCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::SphereCollider>().bodyID);
				else if (entity.HasComponent<Component::CapsuleCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::CapsuleCollider>().bodyID);

				if (rbiter != mRigidBodyIDs.end())
				{
					rb = rbiter->second;
					physx::PxShape* shape[3];
					auto numShapes{ rb->getNbShapes() };
					rb->getShapes(shape, numShapes);// assuming that all the rigidbodies only have one shape
					for (unsigned i{}; i < numShapes; ++i) {
						physx::PxMaterial* material;
						shape[i]->getMaterials(&material, 1);
						material->setDynamicFriction(rigidbody.dynamicFriction);
						material->setStaticFriction(rigidbody.staticFriction);
						material->setRestitution(rigidbody.restitution);
					}
				}
			}
			else if (entity.HasComponent<Component::Transform>()) {
				Component::Transform const& transform = entity.GetComponent<Component::Transform>();
				rb = mPhysics->createRigidDynamic(
					//getting from graphics
					physx::PxTransform(ToPxVec3(transform.worldPos), ToPxQuat(transform.worldRot)));
				mScene->addActor(*rb);
			}
			if (!(bool)rigidbody.motionType) {
				rb->setAngularVelocity(rigidbody.angularVelocity);
				rb->setLinearVelocity(rigidbody.velocity);
				rb->setLinearDamping(rigidbody.linearDamping);
				rb->setAngularDamping(rigidbody.linearDamping);
				rb->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rigidbody.IsAxisLocked((int)Component::RigidBody::Axis::X) ? true : false);
				rb->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rigidbody.IsAxisLocked((int)Component::RigidBody::Axis::Y) ? true : false);
				rb->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rigidbody.IsAxisLocked((int)Component::RigidBody::Axis::Z) ? true : false);

				rb->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidbody.IsAngleAxisLocked((int)Component::RigidBody::Axis::X) ? true : false);
				rb->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidbody.IsAngleAxisLocked((int)Component::RigidBody::Axis::Y) ? true : false);
				rb->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidbody.IsAngleAxisLocked((int)Component::RigidBody::Axis::Z) ? true : false);

			}
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, (bool)rigidbody.motionType);
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);

			rigidbody.bodyID = reinterpret_cast<void*>(rb);
			//mRigidBodyIDs.emplace(rigidbody.bodyID, rb);
			RegisterRB(rigidbody.bodyID, rb, entity);

			auto& res = entity.EmplaceOrReplaceComponent<Component::RigidBody>(rigidbody);

			if (rigidbody.jointID) { // means the joint is old, create new one
				res.jointID = 0;
				ChangeRigidBodyVar(entity, Component::RigidBodyVars::JOINT_ENTITY);
			}
			return res;
			//set 
			//bodyinterface.SetFriction(rigidbody.bodyID, rigidbody.friction);
			//bodyinterface.SetRestitution(rigidbody.bodyID, rigidbody.restitution);
			//bodyinterface.SetGravityFactor(rigidbody.bodyID, rigidbody.gravityFactor);
			//bodyinterface.SetAngularVelocity(rigidbody.bodyID, rigidbody.angularVelocity);

			//mPhysicsSystem.OptimizeBroadPhase();

		}
		namespace {
			template <typename _physx_type, typename _collider_component>
			void SetGeom(_physx_type& geom, _collider_component& collider, Component::Transform const& transform, bool newCollider) {
				if constexpr (std::is_same_v<_physx_type, physx::PxBoxGeometry>) {
					if (newCollider) 
						collider.scale = ToPxVec3(ToPhysicsUnits(transform.worldScale));
					geom = _physx_type{ collider.scale };
				}
				else if constexpr (std::is_same_v<_physx_type, physx::PxSphereGeometry>) {
					if (newCollider) 
						collider.radius = Physics::ToPhysicsUnits(std::max({ transform.worldScale.x, transform.worldScale.y, transform.worldScale.z }));
					geom = _physx_type{ collider.radius };
				}
				else if constexpr (std::is_same_v < _physx_type, physx::PxCapsuleGeometry>) {
					if (newCollider) {
						collider.radius = Physics::ToPhysicsUnits(std::max(transform.worldScale.x, transform.worldScale.z));
						collider.halfheight = Physics::ToPhysicsUnits(transform.worldScale.y);
					}
					geom = _physx_type{ collider.radius, collider.halfheight };
				}
			}
		}

		template <typename _physx_type, typename _collider_component>
		void PhysicsSystem::AddShape(physx::PxRigidDynamic* rb, ECS::Entity const& entity, _collider_component& collider, bool newCollider) {
			_physx_type geom{};
			physx::PxTransform xfm{};
			if (entity.HasComponent<Component::Transform>()) {
				Component::Transform const& transform = entity.GetComponent<Component::Transform>();
				//box shape, this will be a box collider
				SetGeom(geom, collider, transform, newCollider);
				//getting from graphics
				xfm = physx::PxTransform(ToPxVec3(transform.worldPos), ToPxQuat(transform.worldRot));

			}
			else {
				//geom = physx::PxBoxGeometry(physx::PxVec3{1});
				xfm = physx::PxTransform(collider.positionOffset);
			}

			physx::PxShape* shape { CreateShape(geom, collider, entity) };
			rb->setGlobalPose(xfm);
			collider.rotationOffset = ToPxQuat(collider.degreeRotationOffsetEuler);
			shape->setLocalPose({ collider.positionOffset, collider.rotationOffset});
			collider.idx = rb->getNbShapes();
			rb->attachShape(*shape);

		}
		template <typename _physx_type, typename _collider_component>
		void PhysicsSystem::AddNewCollider(physx::PxRigidDynamic*& rb, ECS::Entity const& entity, _collider_component& collider, bool newCollider) {
			Component::Transform transform = entity.GetComponent<Component::Transform>();
			if ((glm::abs(transform.worldScale.x) + glm::abs(transform.worldScale.y) + glm::abs(transform.worldScale.z)) <= glm::epsilon<float>()) {
				transform.worldScale = { 1,1,1 }; //temp fix
			}
			_physx_type geom{};
			//getting from graphics
			physx::PxTransform xfm(ToPxVec3(transform.worldPos), ToPxQuat(transform.worldRot));
			SetGeom(geom, collider, transform, newCollider);
			rb = mPhysics->createRigidDynamic(xfm);
			//ugly syntax to get the shape 
			//assumes that there is only one shape (there should only ever be one starting out)
			physx::PxShape* shape { CreateShape(geom, collider, entity) };
			collider.rotationOffset = ToPxQuat(collider.degreeRotationOffsetEuler);
			shape->setLocalPose({ collider.positionOffset, collider.rotationOffset });
			collider.idx = rb->getNbShapes();
			rb->attachShape(*shape);
			mScene->addActor(*rb);
			collider.idx = 0;
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
			//getting from graphics
			if (rb->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
				rb->setKinematicTarget(physx::PxTransform{ ToPxVec3(transform.worldPos), ToPxQuat(transform.worldRot) });
			else
				rb->setGlobalPose(physx::PxTransform{ ToPxVec3(transform.worldPos), ToPxQuat(transform.worldRot) });


		}
		template <typename _physx_type, typename _collider_component>
		_collider_component& PhysicsSystem::AddCollider(ECS::Entity entity, _collider_component collider, bool newCollider) {
			//check to prevent additional shap adding
			//if (entity.HasComponent<Component::Collider>()) return;
			physx::PxRigidDynamic* rb{};
			if (HasAnyComponent<Component::RigidBody, Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(entity)) {
				auto rbiter{ mRigidBodyIDs.find(nullptr) };
				if (entity.HasComponent<Component::RigidBody>()) 
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::RigidBody>().bodyID);
				else if (entity.HasComponent<Component::BoxCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::BoxCollider>().bodyID);
				else if (entity.HasComponent<Component::SphereCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::SphereCollider>().bodyID);
				else if (entity.HasComponent<Component::CapsuleCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::CapsuleCollider>().bodyID);

				if (rbiter != mRigidBodyIDs.end()) {
					rb = rbiter->second;

				}
				else throw std::runtime_error{ std::string("there is no rigidbody ") };

				AddShape<_physx_type>(rb, entity, collider, newCollider);
			}
			else if (entity.HasComponent<Component::Transform>()) { // this is a given
				AddNewCollider<_physx_type>(rb, entity, collider, newCollider);
				//mRigidBodyIDs.emplace(rb, rb);
				RegisterRB(reinterpret_cast<void*>(rb), rb, entity);
			}
			else {
				throw std::runtime_error{"cannot have no transform or rigidbody components!!"};
			}
			collider.bodyID = reinterpret_cast<void*>(rb);
			return entity.EmplaceOrReplaceComponent<_collider_component>(collider);
		}

		template<typename _physx_type, typename _collider_component>
		physx::PxShape* PhysicsSystem::CreateShape(_physx_type const& geom, _collider_component const& collider, ECS::Entity entity)
		{
			physx::PxShape* shapeptr{ mPhysics->createShape(geom, *mMaterial, true) };

			IGE_LAYERMGR.SetupShapeFilterData(&shapeptr, entity);
			SetColliderAsSensor(shapeptr, collider.sensor);
			return shapeptr;
		}

		Component::BoxCollider& PhysicsSystem::AddBoxCollider(ECS::Entity entity, bool newCollider, Component::BoxCollider collider)
		{
			return AddCollider<physx::PxBoxGeometry>(entity, collider, newCollider);
		}
		Component::SphereCollider& PhysicsSystem::AddSphereCollider(ECS::Entity entity, bool newCollider, Component::SphereCollider collider)
		{
			return AddCollider<physx::PxSphereGeometry>(entity, collider, newCollider);
		}
		Component::CapsuleCollider& PhysicsSystem::AddCapsuleCollider(ECS::Entity entity, bool newCollider, Component::CapsuleCollider collider)
		{
			return AddCollider<physx::PxCapsuleGeometry>(entity, collider, newCollider);
		}

		void PhysicsSystem::CreateJoint(ECS::Entity entity, Component::RigidBody& rb, physx::PxRigidDynamic* rbptr, physx::PxRigidDynamic* otherRbptr) {
			physx::PxTransform localFrameA(rb.jointOffset); // relative to rbA
			physx::PxTransform worldFrame(rb.entityLinkedJointOffset);   // world anchor

			// Create the joint with rbA attached and the other actor set to nullptr.
			physx::PxJoint* joint{ };
			switch (rb.jointConfig.jointType) {
			case Component::RigidBody::JointType::REVOLUTE:
				joint = dynamic_cast<physx::PxJoint*>(physx::PxRevoluteJointCreate(
					*mPhysics,
					rbptr,           // First actor (rbA)
					localFrameA,   // Joint frame relative to rbA
					otherRbptr,       // Second actor is nullptr -> attached to the world
					worldFrame     // World frame for the joint
				));
				break;
			case Component::RigidBody::JointType::SPHERICAL:
				joint = dynamic_cast<physx::PxJoint*>(physx::PxSphericalJointCreate(
					*mPhysics,
					rbptr,           // First actor (rbA)
					localFrameA,   // Joint frame relative to rbA
					otherRbptr,       // Second actor is nullptr -> attached to the world
					worldFrame     // World frame for the joint
				));
				break;
			case Component::RigidBody::JointType::PRISMATIC:
				joint = dynamic_cast<physx::PxJoint*>(physx::PxPrismaticJointCreate(
					*mPhysics,
					rbptr,           // First actor (rbA)
					localFrameA,   // Joint frame relative to rbA
					otherRbptr,       // Second actor is nullptr -> attached to the world
					worldFrame     // World frame for the joint
				));
				break;
			case Component::RigidBody::JointType::DISTANCE:
				joint = dynamic_cast<physx::PxJoint*>(physx::PxDistanceJointCreate(
					*mPhysics,
					rbptr,           // First actor (rbA)
					localFrameA,   // Joint frame relative to rbA
					otherRbptr,       // Second actor is nullptr -> attached to the world
					worldFrame     // World frame for the joint
				));
				break;
			}

			rb.jointID = reinterpret_cast<void*>(joint);
			UpdateJointConfig(entity, rb);

			//add it to the joint map
			mJointMap[reinterpret_cast<void*>(otherRbptr)].emplace(entity.GetEntityID());
		}

		void PhysicsSystem::UpdateJointConfig(ECS::Entity entity, Component::RigidBody& rb) {
			auto& config = rb.jointConfig;
			switch (config.jointType)
			{
			case Component::RigidBody::JointType::REVOLUTE:
			{
				// Cast to a revolute joint and update hinge-specific properties.
				auto joint = reinterpret_cast<physx::PxRevoluteJoint*>(rb.jointID);
				joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
				joint->setBreakForce(config.breakForce, config.breakTorque);

				// Create an angular limit pair for the hinge.
				physx::PxJointAngularLimitPair limits(config.lowerAngle, config.upperAngle);
				limits.stiffness = config.stiffness;
				limits.damping = config.damping;
				joint->setLimit(limits);

				// Motor (drive) settings.
				if (config.motorEnabled)
				{
					joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
					joint->setDriveVelocity(config.motorTargetVelocity);
					joint->setDriveForceLimit(config.motorForceLimit);
				}
				else
				{
					joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_ENABLED, false);
				}
				// Update the frame for the first actor (rbA)
				physx::PxTransform frameA{ rb.jointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, frameA);

				// Update the frame for the second actor (rbB)
				physx::PxTransform frameB{ rb.entityLinkedJointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, frameB);
				break;
			}

			case Component::RigidBody::JointType::SPHERICAL:
			{
				// Cast to a spherical joint and update spherical-specific limits.
				auto joint = reinterpret_cast<physx::PxSphericalJoint*>(rb.jointID);
				joint->setSphericalJointFlag(physx::PxSphericalJointFlag::eLIMIT_ENABLED, true);
				joint->setBreakForce(config.breakForce, config.breakTorque);

				// For spherical joints, use a limit cone.
				// Here, 'yLimit' and 'zLimit' are used to create the cone limit.
				physx::PxJointLimitCone limitCone(config.yLimit, config.zLimit, physx::PxSpring(config.stiffness, config.damping));
				joint->setLimitCone(limitCone);
				// Update the frame for the first actor (rbA)
				physx::PxTransform frameA{ rb.jointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, frameA);

				// Update the frame for the second actor (rbB)
				physx::PxTransform frameB{ rb.entityLinkedJointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, frameB);
				break;
			}

			case Component::RigidBody::JointType::PRISMATIC:
			{
				// Cast to a prismatic joint and update linear limits.
				auto joint = reinterpret_cast<physx::PxPrismaticJoint*>(rb.jointID);
				joint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eLIMIT_ENABLED, true);
				joint->setBreakForce(config.breakForce, config.breakTorque);

				// Create a linear limit pair for prismatic joints.
				physx::PxJointLinearLimitPair limits(config.lowerLimit, config.upperLimit, physx::PxSpring(config.stiffness, config.damping));
				joint->setLimit(limits);
				// Update the frame for the first actor (rbA)
				physx::PxTransform frameA{ rb.jointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, frameA);

				// Update the frame for the second actor (rbB)
				physx::PxTransform frameB{ rb.entityLinkedJointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, frameB);
				break;
			}

			case Component::RigidBody::JointType::DISTANCE:
			{
				// Cast to a distance joint and update linear limits.
				auto joint = reinterpret_cast<physx::PxDistanceJoint*>(rb.jointID);
				joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
				joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
				joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, true);

				joint->setBreakForce(config.breakForce, config.breakTorque);

				// Set a linear limit pair for the distance joint.
				joint->setMaxDistance(config.upperLimit);
				joint->setMinDistance(config.lowerLimit);
				joint->setStiffness(config.stiffness);
				joint->setDamping(config.damping);
				// Update the frame for the first actor (rbA)
				physx::PxTransform frameA{ rb.jointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, frameA);

				// Update the frame for the second actor (rbB)
				physx::PxTransform frameB{ rb.entityLinkedJointOffset };
				joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, frameB);
				break;
			}

			default:
				// Optionally handle unexpected joint types.
				break;
			}


		}

		void PhysicsSystem::RemoveJoint(ECS::Entity entity, Component::RigidBody& rb) {
			if (rb.jointID) {
				auto joint{ reinterpret_cast<physx::PxJoint*>(rb.jointID) };
				joint->release();
			}

			rb.entityLinked = static_cast<unsigned>(-1);
			rb.jointID = 0;	//remove the jointID
		}
		
		void PhysicsSystem::ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var)
		{
			auto& rb{ entity.GetComponent<Component::RigidBody>() };
			auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
			if (rbiter != mRigidBodyIDs.end()) {
				physx::PxRigidDynamic* rbptr{ rbiter->second };

				switch (var) {
				case Component::RigidBodyVars::MASS: {
					rbptr->setMass(rb.mass);
					return;
				}break;
				case Component::RigidBodyVars::GRAVITY_FACTOR: {
					//nothing here
					return;
				}break;
				case Component::RigidBodyVars::VELOCITY: {
					rbptr->setLinearVelocity(rb.velocity);
					return;
				}break;
				case Component::RigidBodyVars::LINEAR_DAMPING: {
					rbptr->setLinearDamping(rb.linearDamping);
					rbptr->setAngularDamping(rb.linearDamping);
				}break;
				case Component::RigidBodyVars::ANGULAR_VELOCITY: {
					rbptr->setAngularVelocity(rb.angularVelocity);
					return;
				}break;
				case Component::RigidBodyVars::MOTION: {
					rbptr->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, (bool)rb.motionType);
					return;
				}break;
				case Component::RigidBodyVars::LOCK: {
					rbptr->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rb.IsAxisLocked((int)Component::RigidBody::Axis::X) ? true : false);
					rbptr->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rb.IsAxisLocked((int)Component::RigidBody::Axis::Y) ? true : false);
					rbptr->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rb.IsAxisLocked((int)Component::RigidBody::Axis::Z) ? true : false);

					rbptr->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rb.IsAngleAxisLocked((int)Component::RigidBody::Axis::X) ? true : false);
					rbptr->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rb.IsAngleAxisLocked((int)Component::RigidBody::Axis::Y) ? true : false);
					rbptr->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rb.IsAngleAxisLocked((int)Component::RigidBody::Axis::Z) ? true : false);

				}break;
				case Component::RigidBodyVars::FORCE: {
					//force is converted to kN
					rbptr->addForce(rb.force * 1000.f * Performance::FrameRateController::GetInstance().GetDeltaTime(), physx::PxForceMode::eFORCE);
					//printf("force added: %d %d %d\n", force.x, force.y, force.z);
				}break;
				case Component::RigidBodyVars::HAS_JOINT: {
					if (rb.hasJoint && !rb.jointID) { //add the joint{
						CreateJoint(entity, rb, rbptr, nullptr);
					}
					else {
						RemoveJoint(entity, rb);
					}
				}break;
				case Component::RigidBodyVars::JOINT_TYPE:
				case Component::RigidBodyVars::JOINT_ENTITY: {
					if (rb.entityLinked != static_cast<unsigned>(-1)) {
						ECS::Entity entityLinked{ ECS::Entity::EntityID{rb.entityLinked} };
						auto otherRbptr = GetRBIter(entityLinked);
						if (otherRbptr) {
							auto saveEntity = rb.entityLinked;
							RemoveJoint(entity, rb);
							CreateJoint(entity, rb, rbptr, otherRbptr);
							rb.entityLinked = saveEntity;
						}
					}
				} break;
				case Component::RigidBodyVars::JOINT_PROPS: {
					if (rb.jointID) {
						UpdateJointConfig(entity, rb);
					}
				}
				}
				if (entity.HasComponent<Component::BoxCollider>() || entity.HasComponent<Component::CapsuleCollider>() || entity.HasComponent<Component::SphereCollider>()) {
					physx::PxShape* shape[3]{};
					auto shapecount{ rbptr->getNbShapes() };
					rbptr->getShapes(shape, 3);// assuming that all the rigidbodies only have one shape

					for (unsigned i{}; i < shapecount; ++i) {
						physx::PxMaterial* material;
						shape[i]->getMaterials(&material, 1);
						switch (var) {
						case Component::RigidBodyVars::STATIC_FRICTION: {
							material->setStaticFriction(rb.staticFriction);
						}break;
						case Component::RigidBodyVars::DYNAMIC_FRICTION: {
							material->setDynamicFriction(rb.dynamicFriction);
						}break;
						case Component::RigidBodyVars::RESTITUTION: {
							material->setRestitution(rb.restitution);
						}break;
						}
					}
				}
			}
			//AddRigidBody(ECS::Entity{});
		}
		template <typename _component_type>
		physx::PxShape* PhysicsSystem::GetShapePtr(_component_type const& collider) {
			auto rbiter{ mRigidBodyIDs.find(collider.bodyID) };
			if (rbiter == mRigidBodyIDs.end()) throw std::runtime_error(" no such bodyID");
			auto rb{ rbiter->second };
			physx::PxU32 numShapes = rb->getNbShapes();
			physx::PxShape* shapes[3]; // max only 3 
			rb->getShapes(shapes, numShapes);
			return shapes[collider.idx];
		}
		void SetColliderAsSensor(physx::PxShape* shapeptr, bool sensor) {
			//if it is a sensor, the shape can be passed thru
			shapeptr->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !sensor);
			shapeptr->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, sensor);

		}
		void PhysicsSystem::ChangeBoxColliderVar(ECS::Entity entity)
		{
			auto& collider{ entity.GetComponent<Component::BoxCollider>() };
			auto shapeptr { GetShapePtr<Component::BoxCollider>(collider) };
			collider.rotationOffset = ToPxQuat(collider.degreeRotationOffsetEuler);
			shapeptr->setLocalPose(physx::PxTransform(collider.positionOffset, collider.rotationOffset));
			shapeptr->setGeometry(physx::PxBoxGeometry{ collider.scale });
			SetColliderAsSensor(shapeptr, collider.sensor);
		}

		void PhysicsSystem::ChangeSphereColliderVar(ECS::Entity entity)
		{
			auto& collider{ entity.GetComponent<Component::SphereCollider>() };
			auto shapeptr{ GetShapePtr<Component::SphereCollider>(collider) };
			auto shapeid{ shapeptr->getGeometryType()};
			collider.rotationOffset = ToPxQuat(collider.degreeRotationOffsetEuler);
			shapeptr->setLocalPose(physx::PxTransform(collider.positionOffset, collider.rotationOffset));
			shapeptr->setGeometry(physx::PxSphereGeometry{ collider.radius });
			SetColliderAsSensor(shapeptr, collider.sensor);
		}

		void PhysicsSystem::ChangeCapsuleColliderVar(ECS::Entity entity)
		{
			auto& collider{ entity.GetComponent<Component::CapsuleCollider>() };
			auto shapeptr{ GetShapePtr<Component::CapsuleCollider>(collider) };
			collider.rotationOffset = ToPxQuat(collider.degreeRotationOffsetEuler);
			shapeptr->setLocalPose(physx::PxTransform(collider.positionOffset, collider.rotationOffset));
			shapeptr->setGeometry(physx::PxCapsuleGeometry{ collider.radius, collider.halfheight });
			SetColliderAsSensor(shapeptr, collider.sensor);
		}

		void PhysicsSystem::Debug() {
			if (Input::InputManager::GetInstance().IsKeyHeld(KEY_CODE::KEY_LEFT_CONTROL) &&
				Input::InputManager::GetInstance().IsKeyTriggered(KEY_CODE::KEY_D)) {
				mDrawDebug = !mDrawDebug;

			}
			if (!mDrawDebug) return;
			{
				auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::BoxCollider>() };
				for (auto entity : rbsystem) {

					auto& collider{ rbsystem.get<Component::BoxCollider>(entity) };
					ECS::Entity e{ entity };
					glm::vec4 debugactivecolor{ e.IsActive() ? glm::vec4{ 0,1,0,1 } : glm::vec4{0.5, 0.5, 0.5, 1} };
					auto rbiter{ mRigidBodyIDs.find(e.GetComponent<Component::BoxCollider>().bodyID) };

					if (rbiter != mRigidBodyIDs.end()) {
						physx::PxRigidDynamic* pxrb { rbiter->second };
						physx::PxShape* shape[3];
						physx::PxBoxGeometry geom{};
						auto num{ pxrb->getNbShapes() };
						if (collider.idx < num) {
							pxrb->getShapes(shape, 3);
							auto globPos{ pxrb->getGlobalPose() };
							if (pxrb->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
								pxrb->getKinematicTarget(globPos);
							physx::PxTransform locPos{ shape[collider.idx]->getLocalPose() };

							shape[collider.idx]->getBoxGeometry(geom);
							auto scale{ geom.halfExtents };
							auto shapeGlobPos(globPos * locPos);

							Graphics::Renderer::DrawBox(ToGLMVec3(shapeGlobPos.p), ToGraphicUnits(ToGLMVec3(scale)), ToGLMQuat(shapeGlobPos.q), debugactivecolor);
						}
					}

				}
			}
			{
				auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::SphereCollider>() };
				for (auto entity : rbsystem) {
					auto& collider{ rbsystem.get<Component::SphereCollider>(entity) };
					ECS::Entity e{ entity };
					glm::vec4 debugactivecolor{ e.IsActive() ? glm::vec4{ 0,1,0,1 } : glm::vec4{0.5, 0.5, 0.5, 1} };

					auto rbiter{ mRigidBodyIDs.find(e.GetComponent<Component::SphereCollider>().bodyID) };

					if (rbiter != mRigidBodyIDs.end()) {
						physx::PxRigidDynamic* pxrb { rbiter->second };
						physx::PxShape* shape[3];
						physx::PxSphereGeometry geom{};
						auto num{ pxrb->getNbShapes() };
						if (collider.idx < num) {
							pxrb->getShapes(shape, 3);
							auto globPos{ pxrb->getGlobalPose() };
							if (pxrb->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
								pxrb->getKinematicTarget(globPos);
							physx::PxTransform locPos{ shape[collider.idx]->getLocalPose() };

							shape[collider.idx]->getSphereGeometry(geom);
							auto radius{ geom.radius };
							auto shapeGlobPos(globPos * locPos);
							//i think the radius is actually the diameter here so im gonna divide by 2
							Graphics::Renderer::DrawWireSphere(ToGLMVec3(shapeGlobPos.p), ToGraphicUnits(radius/2.f), debugactivecolor, 16);
						}
					}

				}
			}
			{
				auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::CapsuleCollider>() };
				for (auto entity : rbsystem) {
					auto& collider{ rbsystem.get<Component::CapsuleCollider>(entity) };
					ECS::Entity e{ entity };
					glm::vec4 debugactivecolor{ e.IsActive() ? glm::vec4{ 0,1,0,1 } : glm::vec4{0.5, 0.5, 0.5, 1} };
					auto rbiter{ mRigidBodyIDs.find(e.GetComponent<Component::CapsuleCollider>().bodyID) };

					if (rbiter != mRigidBodyIDs.end()) {
						physx::PxRigidDynamic* pxrb { rbiter->second };
						physx::PxShape* shape[3];
						physx::PxCapsuleGeometry geom{};
						auto num{ pxrb->getNbShapes() };
						if (collider.idx < num) {
							pxrb->getShapes(shape, 3);
							auto globPos{ pxrb->getGlobalPose() };
							if (pxrb->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
								pxrb->getKinematicTarget(globPos);
							physx::PxTransform locPos{ shape[collider.idx]->getLocalPose() };

							shape[collider.idx]->getCapsuleGeometry(geom);
							auto shapeGlobPos(globPos * locPos);

							glm::mat4 xfm{1};
							{//get the capsule matrix
								auto pos{ ToGLMVec3(shapeGlobPos.p) };

								auto rot{ ToGLMQuat(shapeGlobPos.q) };
								// create a 90 degree offset in the z axis
								// cuz the draw is rotated -90 in the z axis
								glm::quat zRot = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
								rot = rot * zRot; // Apply Z rotation first, then the original rotation

								auto scale{ glm::vec3{1} }; // scale of 1 just to not modify stuff

								xfm = glm::translate(xfm, pos);
								xfm *= glm::toMat4(rot);
								xfm = glm::scale(xfm, scale);
							}
							//same thing here i think the radius is actually diameter
							//half height converted to height for the param
							Graphics::Renderer::DrawWireCapsule(xfm, ToGraphicUnits(geom.radius/2.f), ToGraphicUnits(geom.halfHeight * 2.f), debugactivecolor);
						}
					}

				}
			}
			{ // debug for joints
				auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::RigidBody>() };
				for (auto entity : rbsystem) {
					auto const& rb = rbsystem.get<Component::RigidBody>(entity);
					ECS::Entity e0{ entity };
					ECS::Entity e1{ ECS::Entity::EntityID(rb.entityLinked) };

					if (rb.hasJoint && rb.jointID) {
						// Retrieve the joint's local poses for actor0 and actor1
						auto localPose0 = reinterpret_cast<physx::PxJoint*>(rb.jointID)->getLocalPose(physx::PxJointActorIndex::eACTOR0);
						auto localPose1 = reinterpret_cast<physx::PxJoint*>(rb.jointID)->getLocalPose(physx::PxJointActorIndex::eACTOR1);

						// Retrieve the global poses of the actors
						auto globalPose0 = reinterpret_cast<physx::PxRigidDynamic*>(rb.bodyID)->getGlobalPose();
						physx::PxTransform globalPose1{};
						if (rb.entityLinked != (unsigned)-1) {
							auto rbiter1 = GetRBIter(e1);
							if (rbiter1)
								globalPose1 = rbiter1->getGlobalPose();
						}

						// Combine transforms to get the joint frame in world space.
						// This properly applies the rotation and translation of the joint relative to each actor.
						auto worldJointPose0 = globalPose0 * localPose0;
						auto worldJointPose1 = globalPose1 * localPose1;

						auto jointClr = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
						switch (rb.jointConfig.jointType) {
						case Component::RigidBody::JointType::SPHERICAL:
						case Component::RigidBody::JointType::REVOLUTE: {
							// Draw a line from the joint frame on actor0 to actor0's global origin.
							Graphics::Renderer::DrawLine(ToGLMVec3(worldJointPose0.p), ToGLMVec3(globalPose0.p), jointClr);
							// Draw a line from the joint frame on actor1 to actor1's global origin.
							Graphics::Renderer::DrawLine(ToGLMVec3(worldJointPose1.p), ToGLMVec3(globalPose1.p), jointClr);
							// Draw a sphere at the joint frame on actor1.
							Graphics::Renderer::DrawWireSphere(ToGLMVec3(worldJointPose1.p), 0.1f, jointClr);
						} break;
						case Component::RigidBody::JointType::PRISMATIC:
						case Component::RigidBody::JointType::DISTANCE: {
							// Draw spheres at both joint frames.
							Graphics::Renderer::DrawWireSphere(ToGLMVec3(worldJointPose0.p), 0.1f, jointClr);
							Graphics::Renderer::DrawWireSphere(ToGLMVec3(worldJointPose1.p), 0.1f, jointClr);
							// Draw a line connecting the two joint frames.
							Graphics::Renderer::DrawLine(ToGLMVec3(worldJointPose0.p), ToGLMVec3(worldJointPose1.p), jointClr);
						} break;
						}
					}
				}
			}
			for (auto const& ray : mRays) {
				if (ray.detected) {
					//draw three lines
					auto dir{ glm::normalize(ray.end - ray.origin) };
					auto intersectPoint{ ray.origin + (dir * ray.hit.distance) };
					Graphics::Renderer::DrawLine(ray.origin, intersectPoint, { 0.f, 1.f, 0.f, 1.f });
					Graphics::Renderer::DrawLine(intersectPoint, ray.end, { 1.f, 0.f, 0.f, 1.f });
				}
				else {
					Graphics::Renderer::DrawLine(ray.origin, ray.end, { 0.f, 1.f, 0.f, 1.f });
				}
			}
			for (auto const& ray : mGeneralRays) {
				//draw three lines
				auto dir{ glm::normalize(ray.end - ray.start) };
				Graphics::Renderer::DrawLine(ray.start, ray.end, { 1.f, 0.f, 1.f, 1.f });
				Graphics::Renderer::DrawWireSphere(ray.start, 0.1f, { 1.f, 0.f, 1.f, 1.f });
				Graphics::Renderer::DrawWireSphere(ray.end, 0.1f, { 1.f, 0.f, 1.f, 1.f });

			}
			mGeneralRays.clear();
			mRays.clear();
		}

		void PhysicsSystem::ClearSystem()
		{
			mScene->setSimulationEventCallback(nullptr); // unsubscribe from all events first
			for (auto rbpair : mRigidBodyIDs) {
				auto rb{ rbpair.second };
				mScene->removeActor(*rb);
				rb->release();
			}
			mRigidBodyIDs.clear();
			mJointMap.clear();
			mRigidBodyToEntity.clear();
			mInactiveActors.clear();
			mOnTriggerPairs.clear();
			mOnContactPairs.clear();
			mScene->simulate(0.0f);
			mScene->fetchResults(true);
			mScene->setSimulationEventCallback(mEventManager);
		}

		bool PhysicsSystem::RayCastSingular(glm::vec3 const& origin, glm::vec3 const& end, RaycastHit& result)
		{
			physx::PxRaycastBufferN<8> hitBuffer{};
			auto dir{ glm::normalize(end - origin) };
			auto mag{ glm::distance(origin, end) };
			bool hit{ mScene->raycast(ToPxVec3(origin), ToPxVec3(dir), mag, hitBuffer) };
			if (hit) {
				std::sort(hitBuffer.touches, hitBuffer.touches + hitBuffer.getNbTouches(), [](const physx::PxRaycastHit& a, const physx::PxRaycastHit& b) {
					return a.distance < b.distance;
					});
				result.entity = mRigidBodyToEntity.at(reinterpret_cast<void*>(hitBuffer.touches[0].actor));
				result.distance = hitBuffer.touches[0].distance;
				result.normal = ToGLMVec3(hitBuffer.touches[0].normal);
				result.position = ToGLMVec3(hitBuffer.touches[0].position);
				if (mDrawDebug)
					mRays.emplace_back(RayCastResult{
							origin, end, result, true
						});
				return true;
			}
			if (mDrawDebug)
				mRays.emplace_back(RayCastResult{
						origin, end, result, false
					});
			return false;
		}

		bool PhysicsSystem::RayCastFromEntity(ECS::Entity entity, glm::vec3 const& direction, float magnitude, RaycastHit& result)
		{
			physx::PxRigidDynamic* rb{ GetRBIter(entity)};
			if (!rb) return false;
			auto origin{ ToGLMVec3(rb->getGlobalPose().p) };
			auto end{ origin + glm::normalize(direction) * magnitude };
			return RayCastFromEntity(entity, origin, end, result);
		}

		bool PhysicsSystem::RayCastFromEntity(ECS::Entity entity, glm::vec3 const& origin, glm::vec3 const& end, RaycastHit& result)
		{
			//hit buffer is 2 to account for in case the ray hits the origin entity
			const int sz{ 8 };
			physx::PxRaycastBufferN<sz> hitBuffer{};
			auto dir{ glm::normalize(end - origin) };
			auto magnitude{ glm::distance(origin, end) };
			bool hit{ mScene->raycast(ToPxVec3(origin), ToPxVec3(dir), magnitude, hitBuffer) };
			if (hit) {
				std::sort(hitBuffer.touches, hitBuffer.touches + hitBuffer.getNbTouches(), [](const physx::PxRaycastHit& a, const physx::PxRaycastHit& b) {
					return a.distance < b.distance;
					});

				//sort the distance of the objects
				
				for (uint64_t idx{}; idx < hitBuffer.nbTouches; ++idx) {
					auto const& e{ mRigidBodyToEntity.at(reinterpret_cast<void*>(hitBuffer.touches[idx].actor)) };
					if (e == entity) { // if entity1 is the original entity use the other
						continue;
					}
					if (!e.IsActive()) {
						continue;
					}
					//std::cout << "from physsystem " << e.GetComponent<Component::Tag>().tag << " " << e.IsActive() << std::endl;
					result.entity = e;
					result.distance = hitBuffer.touches[idx].distance;
					result.normal = ToGLMVec3(hitBuffer.touches[idx].normal);
					result.position = ToGLMVec3(hitBuffer.touches[idx].position);
					if (mDrawDebug)
						mRays.emplace_back(RayCastResult{
								origin, end, result, true
							});
					return true;
				}
			}
			if (mDrawDebug)
				mRays.emplace_back(RayCastResult{
						origin, end, result, false
					});
			return false;
		}
		
		physx::PxRigidDynamic* PhysicsSystem::GetRBIter(ECS::Entity entity) {
			auto rbiter{ mRigidBodyIDs.find(nullptr) };
			if (HasAnyComponent<Component::RigidBody, Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(entity)) {	
				if (entity.HasComponent<Component::RigidBody>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::RigidBody>().bodyID);
				else if (entity.HasComponent<Component::BoxCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::BoxCollider>().bodyID);
				else if (entity.HasComponent<Component::SphereCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::SphereCollider>().bodyID);
				else if (entity.HasComponent<Component::CapsuleCollider>())
					rbiter = mRigidBodyIDs.find(entity.GetComponent<Component::CapsuleCollider>().bodyID);
				return rbiter->second;
			}
			return nullptr;
		}
		bool PhysicsSystem::OnTriggerEnter(ECS::Entity trigger, ECS::Entity other) {
			auto triggerrb{ reinterpret_cast<void*>(GetRBIter(trigger)) };
			auto otherrb{ reinterpret_cast<void*>(GetRBIter(other)) };
			if (triggerrb && otherrb) {
				return (
					mOnTriggerPairs.find(triggerrb) != mOnTriggerPairs.end() &&
					mOnTriggerPairs.at(triggerrb).find(otherrb) != mOnTriggerPairs.at(triggerrb).end() &&
					mOnTriggerPairs.at(triggerrb).at(otherrb) == (int)TriggerResult::FOUND
				);
			}
			return false;
		}

		bool PhysicsSystem::OnTriggerExit(ECS::Entity trigger, ECS::Entity other) {
			auto triggerrb{ reinterpret_cast<void*>(GetRBIter(trigger)) };
			auto otherrb{ reinterpret_cast<void*>(GetRBIter(other)) };
			if (triggerrb && otherrb) {
				return (
					mOnTriggerPairs.find(triggerrb) != mOnTriggerPairs.end() &&
					mOnTriggerPairs.at(triggerrb).find(otherrb) != mOnTriggerPairs.at(triggerrb).end() &&
					mOnTriggerPairs.at(triggerrb).at(otherrb) == (int)TriggerResult::LOST
					);
			}
			return false;
		}

		std::vector<physx::PxContactPairPoint> PhysicsSystem::GetContactPoints(ECS::Entity entity1, ECS::Entity entity2)
		{
			void* firstactor{  reinterpret_cast<void*>(GetRBIter(entity1)) };
			void* secondactor{ reinterpret_cast<void*>(GetRBIter(entity2)) };
			
			if (!firstactor || !secondactor)
				return std::vector<physx::PxContactPairPoint>();

			if (mOnContactPairs.find(firstactor) != mOnContactPairs.end() && mOnContactPairs[firstactor].find(secondactor) != mOnContactPairs[firstactor].end()) {
				return mOnContactPairs[firstactor][secondactor];
			}
			if (mOnContactPairs.find(secondactor) != mOnContactPairs.end() && mOnContactPairs[secondactor].find(firstactor) != mOnContactPairs[secondactor].end()) {
				return mOnContactPairs[secondactor][firstactor];
			}
			return std::vector<physx::PxContactPairPoint>{};
		}


		bool PhysicsSystem::OnTriggerPersists(ECS::Entity trigger, ECS::Entity other)
		{
			auto triggerrb{ reinterpret_cast<void*>(GetRBIter(trigger)) };
			auto otherrb{ reinterpret_cast<void*>(GetRBIter(other)) };
			if (triggerrb && otherrb) {
				return (
					mOnTriggerPairs.find(triggerrb) != mOnTriggerPairs.end() &&
					mOnTriggerPairs.at(triggerrb).find(otherrb) != mOnTriggerPairs.at(triggerrb).end() &&
					mOnTriggerPairs.at(triggerrb).at(otherrb) == (int)TriggerResult::PERSISTS
					);
			}
			return false;
		}

		float PhysicsSystem::GetShortestDistance(ECS::Entity e1, ECS::Entity e2)
		{
			// Require both entities to have a Transform.
			if (!e1.HasComponent<Component::Transform>() || !e2.HasComponent<Component::Transform>())
				return -1.f;

			// Get the transforms.
			auto t1 = e1.GetComponent<Component::Transform>();
			auto t2 = e2.GetComponent<Component::Transform>();

			float bestSqDist = FLT_MAX;
			glm::vec3 bestStart{ 0.f };
			glm::vec3 bestEnd{ 0.f };
			bool found = false;

			// Helper lambda: Given a point and a collider (its geometry and pose),
			// query the squared distance and closest point on the collider.
			auto queryPointToCollider = [&bestSqDist, &bestStart, &bestEnd, &found](const physx::PxVec3& point,
				const physx::PxGeometry& geom, const physx::PxTransform& pose)
				{
					physx::PxVec3 closestPt;
					// pointDistance returns the squared distance.
					physx::PxReal sqDist = physx::PxGeometryQuery::pointDistance(point, geom, pose, &closestPt);
					if (sqDist >= 0.f && sqDist < bestSqDist)
					{
						bestSqDist = sqDist;
						bestStart = ToGLMVec3(point);
						bestEnd = ToGLMVec3(closestPt);
						found = true;
					}
				};

			// For each collider component on e1, compare against each collider component on e2.
			// We consider the "center" of each collider to be: transform.worldPos + collider.positionOffset.
			// And we build the collider�s world pose using the entity�s rotation combined with the collider�s rotation offset.

			// --- e1: BoxCollider
			if (e1.HasComponent<Component::BoxCollider>())
			{
				auto box1 = e1.GetComponent<Component::BoxCollider>();
				physx::PxBoxGeometry geom1(box1.scale); // 'scale' is assumed to be the half-extents.
				physx::PxVec3 center1 = ToPxVec3(t1.worldPos) + box1.positionOffset;
				physx::PxTransform pose1(center1, ToPxQuat(t1.worldRot) * box1.rotationOffset);

				// Compare with e2's colliders.
				if (e2.HasComponent<Component::BoxCollider>())
				{
					auto box2 = e2.GetComponent<Component::BoxCollider>();
					physx::PxBoxGeometry geom2(box2.scale);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + box2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * box2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
				if (e2.HasComponent<Component::SphereCollider>())
				{
					auto sphere2 = e2.GetComponent<Component::SphereCollider>();
					physx::PxSphereGeometry geom2(sphere2.radius);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + sphere2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * sphere2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
				if (e2.HasComponent<Component::CapsuleCollider>())
				{
					auto capsule2 = e2.GetComponent<Component::CapsuleCollider>();
					physx::PxCapsuleGeometry geom2(capsule2.radius, capsule2.halfheight);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + capsule2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * capsule2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
			}

			// --- e1: SphereCollider
			if (e1.HasComponent<Component::SphereCollider>())
			{
				auto sphere1 = e1.GetComponent<Component::SphereCollider>();
				physx::PxSphereGeometry geom1(sphere1.radius);
				physx::PxVec3 center1 = ToPxVec3(t1.worldPos) + sphere1.positionOffset;
				physx::PxTransform pose1(center1, ToPxQuat(t1.worldRot) * sphere1.rotationOffset);

				if (e2.HasComponent<Component::BoxCollider>())
				{
					auto box2 = e2.GetComponent<Component::BoxCollider>();
					physx::PxBoxGeometry geom2(box2.scale);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + box2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * box2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
				if (e2.HasComponent<Component::SphereCollider>())
				{
					auto sphere2 = e2.GetComponent<Component::SphereCollider>();
					physx::PxSphereGeometry geom2(sphere2.radius);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + sphere2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * sphere2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
				if (e2.HasComponent<Component::CapsuleCollider>())
				{
					auto capsule2 = e2.GetComponent<Component::CapsuleCollider>();
					physx::PxCapsuleGeometry geom2(capsule2.radius, capsule2.halfheight);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + capsule2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * capsule2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
			}

			// --- e1: CapsuleCollider
			if (e1.HasComponent<Component::CapsuleCollider>())
			{
				auto capsule1 = e1.GetComponent<Component::CapsuleCollider>();
				physx::PxCapsuleGeometry geom1(capsule1.radius, capsule1.halfheight);
				physx::PxVec3 center1 = ToPxVec3(t1.worldPos) + capsule1.positionOffset;
				physx::PxTransform pose1(center1, ToPxQuat(t1.worldRot) * capsule1.rotationOffset);

				if (e2.HasComponent<Component::BoxCollider>())
				{
					auto box2 = e2.GetComponent<Component::BoxCollider>();
					physx::PxBoxGeometry geom2(box2.scale);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + box2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * box2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
				if (e2.HasComponent<Component::SphereCollider>())
				{
					auto sphere2 = e2.GetComponent<Component::SphereCollider>();
					physx::PxSphereGeometry geom2(sphere2.radius);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + sphere2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * sphere2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
				if (e2.HasComponent<Component::CapsuleCollider>())
				{
					auto capsule2 = e2.GetComponent<Component::CapsuleCollider>();
					physx::PxCapsuleGeometry geom2(capsule2.radius, capsule2.halfheight);
					physx::PxVec3 center2 = ToPxVec3(t2.worldPos) + capsule2.positionOffset;
					physx::PxTransform pose2(center2, ToPxQuat(t2.worldRot) * capsule2.rotationOffset);
					queryPointToCollider(center1, geom2, pose2);
				}
			}

			// (Optionally, you could also run the symmetric queries � from each collider in e2 against those in e1 �
			// but since distance is symmetric, this is omitted for brevity.)

			// If no valid query was made or the computed gap is non-positive, return -1.
			if (!found || bestSqDist <= 0.f || bestSqDist == FLT_MAX)
				return -1.f;

			float bestDistance = sqrtf(bestSqDist);

			// Add the two points as a GeneralRay.
			// (Assuming mGeneralRays is a data member of this class and accessible here.)
			GeneralRay ray;
			ray.start = bestStart;
			ray.end = bestEnd;
			mGeneralRays.push_back(ray);

			return bestDistance;
		}

		template<typename _physx_type, typename _collider_component>
		void PhysicsSystem::RemoveCollider(ECS::Entity entity, physx::PxGeometryType::Enum typeenum)
		{
			auto const& collider = entity.GetComponent<_collider_component>();
			auto rbiter{ mRigidBodyIDs.find(collider.bodyID) };
			if (rbiter == mRigidBodyIDs.end()) throw std::runtime_error(" no such bodyID");
			auto rb{ rbiter->second };
			physx::PxU32 numShapes = rb->getNbShapes();
			physx::PxShape* shapes[3]; // max only 3 
			rb->getShapes(shapes, numShapes);
			for (physx::PxU32 i = 0; i < numShapes; ++i) {
				physx::PxShape* shape = shapes[i];
				if (shape->getGeometry().getType() == typeenum) {
					rb->detachShape(*shapes[i]);
					shapes[i]->release();
					break;
				}
			}

			if (numShapes == 1 && !entity.HasComponent<Component::Transform>()) {
				mScene->removeActor(*rb);
				rb->release();
				//mRigidBodyIDs.erase(collider.bodyID);
				RemoveRB(collider.bodyID);
			}
			else { // reissue the idx since physx shuffles the shapes after removal
				numShapes = rb->getNbShapes();
				rb->getShapes(shapes, numShapes);
				for (physx::PxU32 i = 0; i < numShapes; ++i) {
					physx::PxShape* shape = shapes[i];
					physx::PxGeometryHolder geometryHolder = shape->getGeometry();
					if (geometryHolder.getType() == physx::PxGeometryType::Enum::eBOX)
						entity.GetComponent<Component::BoxCollider>().idx = i;
					else if (geometryHolder.getType() == physx::PxGeometryType::Enum::eSPHERE)
						entity.GetComponent<Component::SphereCollider>().idx = i;
					else if (geometryHolder.getType() == physx::PxGeometryType::Enum::eCAPSULE)
						entity.GetComponent<Component::CapsuleCollider>().idx = i;
				}
			}

		}
		void PhysicsSystem::RemoveRigidBody(ECS::Entity entity)
		{
			//assuming the check has already been done
			auto& rb{ entity.GetComponent<Component::RigidBody>() };
			//if dont have any colliders just release
			if (!(entity.HasComponent<Component::BoxCollider>() || 
				entity.HasComponent<Component::SphereCollider>() || 
				entity.HasComponent<Component::CapsuleCollider>())) {

				auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
				if (rbiter == mRigidBodyIDs.end()) throw std::runtime_error{ std::string(std::string("no such body id "))};
				mScene->removeActor(*rbiter->second);
				rbiter->second->release();
				//mRigidBodyIDs.erase(rb.bodyID);
				RemoveRB(rb.bodyID);
			}//else let the collider just exist on its own
		}

		void PhysicsSystem::RegisterRB(void* bodyID, physx::PxRigidDynamic* rbptr, ECS::Entity const& entity) noexcept
		{
			mRigidBodyIDs.emplace(bodyID, rbptr);
			mRigidBodyToEntity.emplace(bodyID, entity);
		}

		void PhysicsSystem::RemoveRB(void* bodyID) noexcept
		{
			mRigidBodyIDs.erase(bodyID);
			mRigidBodyToEntity.erase(bodyID);
		}

		PHYSICS_EVENT_LISTENER_IMPL(PhysicsSystem::OnContactSampleListener)
		{
			auto const& pair{ e.GetParam<std::pair<ECS::Entity, ECS::Entity>>(IGE::Physics::EventKey::EventContact::ENTITY_PAIR) };
			auto const& cp{ e.GetParam<std::vector<physx::PxContactPairPoint>>(IGE::Physics::EventKey::EventContact::CONTACT_POINTS) };
		}

		PHYSICS_EVENT_LISTENER_IMPL(PhysicsSystem::OnTriggerSampleListener)
		{
			auto const& trigger{ e.GetParam<ECS::Entity>(IGE::Physics::EventKey::EventTrigger::TRIGGER_ENTITY) };
			auto const& other{ e.GetParam<ECS::Entity>(IGE::Physics::EventKey::EventTrigger::OTHER_ENTITY) };
		}

		EVENT_CALLBACK_DEF(PhysicsSystem, HandleRemoveComponent) {
			auto e{ CAST_TO_EVENT(Events::RemoveComponentEvent) };
			auto rbptr{ GetRBIter(e->mEntity) };
			bool isLastPhysicsComponent{ false };
			if (e->mType == rttr::type::get<Component::RigidBody>()) {
				RemoveRigidBody(e->mEntity);
				isLastPhysicsComponent = !HasAnyComponent<Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(e->mEntity);
			} 
			else if (e->mType == rttr::type::get<Component::BoxCollider>()) {
				RemoveCollider<physx::PxBoxGeometry, Component::BoxCollider>(e->mEntity, physx::PxGeometryType::Enum::eBOX);
				isLastPhysicsComponent = !HasAnyComponent<Component::RigidBody, Component::SphereCollider, Component::CapsuleCollider>(e->mEntity);
			}
			else if (e->mType == rttr::type::get<Component::SphereCollider>()) {
				RemoveCollider<physx::PxSphereGeometry, Component::SphereCollider>(e->mEntity, physx::PxGeometryType::Enum::eSPHERE);
				isLastPhysicsComponent = !HasAnyComponent<Component::BoxCollider, Component::RigidBody, Component::CapsuleCollider>(e->mEntity);
			}
			else if (e->mType == rttr::type::get<Component::CapsuleCollider>()) {
				RemoveCollider<physx::PxCapsuleGeometry, Component::CapsuleCollider>(e->mEntity, physx::PxGeometryType::Enum::eCAPSULE);
				isLastPhysicsComponent = !HasAnyComponent<Component::BoxCollider, Component::SphereCollider, Component::RigidBody>(e->mEntity);
			}

			if (rbptr && 
				isLastPhysicsComponent &&
				mJointMap.find(reinterpret_cast<void*>(rbptr)) != mJointMap.end()) {
				for (auto entityid : mJointMap[rbptr]) {
					ECS::Entity entity{ ECS::Entity::EntityID{entityid} };
					if (ECS::EntityManager::GetInstance().IsValidEntity(entity) && entity.HasComponent<Component::RigidBody>()) {
						entity.GetComponent<Component::RigidBody>().hasJoint = false;
						ChangeRigidBodyVar(entity, Component::RigidBodyVars::HAS_JOINT);
					}
				}
			}
		}
		EVENT_CALLBACK_DEF(PhysicsSystem, HandleRemoveEntity) {
			auto e{ CAST_TO_EVENT(Events::RemoveEntityEvent) };
			auto rbptr{ GetRBIter(e->mEntity) };
			if (e->mEntity.HasComponent<Component::RigidBody>()) {
				RemoveRigidBody(e->mEntity);
			}
			if (e->mEntity.HasComponent<Component::BoxCollider>()) {
				RemoveCollider<physx::PxBoxGeometry, Component::BoxCollider>(e->mEntity, physx::PxGeometryType::Enum::eBOX);
			}
			if (e->mEntity.HasComponent<Component::SphereCollider>()) {
				RemoveCollider<physx::PxSphereGeometry, Component::SphereCollider>(e->mEntity, physx::PxGeometryType::Enum::eSPHERE);
			}
			if (e->mEntity.HasComponent<Component::CapsuleCollider>()) {
				RemoveCollider<physx::PxCapsuleGeometry, Component::CapsuleCollider>(e->mEntity, physx::PxGeometryType::Enum::eCAPSULE);
			}
			if (mJointMap.find(reinterpret_cast<void*>(rbptr)) != mJointMap.end() && rbptr) {
				for (auto entityid : mJointMap[rbptr]) {
					ECS::Entity entity{ ECS::Entity::EntityID{entityid} };
					if (ECS::EntityManager::GetInstance().IsValidEntity(entity) && entity.HasComponent<Component::RigidBody>()) {
						entity.GetComponent<Component::RigidBody>().hasJoint = false;
						ChangeRigidBodyVar(entity, Component::RigidBodyVars::HAS_JOINT);
					}
				}
			}

		}
		PhysicsSystem::~PhysicsSystem() {
			//UnregisterTypes();
			//delete JPH::Factory::sInstance;
			//delete JPH::DebugRenderer::sInstance;
			//Factory::sInstance = nullptr;

			//deleted on release function call
			mScene->release();
			mPhysics->release();
			mPvd->release();
			mFoundation->release();

			delete mEventManager;
		}
	}

	physx::PxFilterFlags LayerFilterShaderWrapper(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {
		pairFlags |=
			physx::PxPairFlag::eCONTACT_DEFAULT |
			physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
			physx::PxPairFlag::eNOTIFY_TOUCH_LOST |
			physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS | 
			physx::PxPairFlag::eNOTIFY_CONTACT_POINTS |
			physx::PxPairFlag::eTRIGGER_DEFAULT |
			physx::PxPairFlag::eDETECT_CCD_CONTACT;

		return IGE_LAYERMGR.LayerFilterShader(
				attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize);
	}
}
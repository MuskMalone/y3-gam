#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include <Core/Components/Components.h>
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Scenes/SceneManager.h"
#include "Physics/PhysicsHelpers.h"
namespace IGE {
	namespace Physics {
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
			mMaterial{ mPhysics->createMaterial(0.5f, 0.5f, 0.6f) }
			//mTempAllocator{ 10 * 1024 * 1024 },
			//mJobSystem{ cMaxPhysicsJobs, cMaxPhysicsBarriers, static_cast<int>(thread::hardware_concurrency() - 1) }
		{
			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			// Create the scene (simulation world)
			physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, 0.f, 0.0f); // Gravity pointing downward

			// Use default CPU dispatcher
			physx::PxDefaultCpuDispatcher* dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = dispatcher;
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			mScene = mPhysics->createScene(sceneDesc);
		}

		void PhysicsSystem::Update() {
			//if (Scenes::SceneManager::GetInstance().GetSceneState() != Scenes::SceneState::PLAYING) {
			//	auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::RigidBody, Component::Transform>() };
			//	for (auto entity : rbsystem) {
			//		auto& xfm{ rbsystem.get<Component::Transform>(entity) };
			//		auto& rb{ rbsystem.get<Component::RigidBody>(entity) };
			//		auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
			//		if (rbiter != mRigidBodyIDs.end()) {
			//			physx::PxRigidDynamic* pxrigidbody{ mRigidBodyIDs.at(rb.bodyID) };
			//			//update positions
			//			pxrigidbody->setGlobalPose(physx::PxTransform{ToPxVec3(xfm.worldPos), ToPxQuat(xfm.worldRot)});
			//		}
			//		//JPH::BodyInterface& bodyInterface { mPhysicsSystem.GetBodyInterface() };
			//		//xfm.worldPos = ToGLMVec3(bodyInterface.GetPosition(rb.bodyID));
			//		//xfm.worldPos = ToGLMVec3(bodyInterface.(rb.bodyID));
			//	}
			//}
			//else {
				//mPhysicsSystem.Update(gDeltaTime, 1, &mTempAllocator, &mJobSystem);
					// Simulate one time step (1/60 second)
			mScene->simulate(gTimeStep);

			// Wait for the simulation to complete
			mScene->fetchResults(true);
			auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::RigidBody, Component::Transform>() };
			for (auto entity : rbsystem) {
				auto& xfm{ rbsystem.get<Component::Transform>(entity) };
				auto& rb{ rbsystem.get<Component::RigidBody>(entity) };
				auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
				if (rbiter != mRigidBodyIDs.end()) {
					physx::PxRigidDynamic* pxrigidbody{ mRigidBodyIDs.at(rb.bodyID) };
					//apply gravity
					if (rb.motionType == Component::RigidBody::MotionType::DYNAMIC) {
						float grav{ gGravity * rb.gravityFactor * rb.mass };
						pxrigidbody->addForce(ToPxVec3(glm::vec3(0.f, grav, 0.f)));
					}
					xfm.worldPos += ToGLMVec3(pxrigidbody->getLinearVelocity() * gTimeStep);
					{
						auto angularVelocity = pxrigidbody->getAngularVelocity();
						float angle = angularVelocity.magnitude() * gTimeStep;
						if (glm::abs(angle) > glm::epsilon<float>()) {
							auto axis = angularVelocity.getNormalized();
							physx::PxQuat deltaRotation(angle, axis);  
							// to add rotations, multiply 2 quats tgt
							xfm.worldRot *= ToGLMQuat(deltaRotation);
						}						
					}
					xfm.modified = true; // include this 
					pxrigidbody->setLinearVelocity(rb.velocity);
					//JPH::BodyInterface& bodyInterface { mPhysicsSystem.GetBodyInterface() };
					//xfm.worldPos = ToGLMVec3(bodyInterface.GetPosition(rb.bodyID));
					//xfm.worldPos = ToGLMVec3(bodyInterface.(rb.bodyID));
				}
			}

		}

		Component::RigidBody& PhysicsSystem::AddRigidBody(ECS::Entity entity, Component::RigidBody rigidbody) {
			//if (entity.HasComponent<Component::RigidBody>()) return;
			//auto& bodyinterface = mPhysicsSystem.GetBodyInterface();

				// Half extents (1 unit per side)
				//physx::PxRigidDynamic* rb = PxCreateDynamic(
				//	*mPhysics, 
				//	physx::PxTransform(ToPxVec3(transform.worldPos)),
				//	physx::PxBoxGeometry(ToPxVec3(transform.worldScale)),
				//	*mMaterial, rigidbody.mass); // Mass = 10.0f
			physx::PxRigidDynamic* rb { }; // Mass = 10.0f
			if (entity.HasComponent<Component::BoxCollider>()) {
				auto const& collider{ entity.GetComponent<Component::BoxCollider>() };

				auto rbiter{ mRigidBodyIDs.find(collider.bodyID) };


				if (rbiter != mRigidBodyIDs.end())
				{
					rb = rbiter->second;
					physx::PxShape* shape;
					physx::PxMaterial* material;
					rb->getShapes(&shape, 1);// assuming that all the rigidbodies only have one shape
					shape->getMaterials(&material, 1);
					material->setDynamicFriction(rigidbody.dynamicFriction);
					material->setStaticFriction(rigidbody.staticFriction);
					material->setRestitution(rigidbody.restitution);

				}
			}
			else if (entity.HasComponent<Component::Transform>()) {
				Component::Transform const& transform = entity.GetComponent<Component::Transform>();
				rb = mPhysics->createRigidDynamic(
					physx::PxTransform(ToPxVec3(transform.worldPos)));
				mScene->addActor(*rb);
			}
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, (bool)rigidbody.motionType);
			rb->setLinearVelocity(rigidbody.velocity);
			rb->setLinearDamping(rigidbody.linearDamping);

			rigidbody.bodyID = reinterpret_cast<void*>(rb);
			mRigidBodyIDs.emplace(rigidbody.bodyID, rb);
			return entity.EmplaceOrReplaceComponent<Component::RigidBody>(rigidbody);

			//set 
			//bodyinterface.SetFriction(rigidbody.bodyID, rigidbody.friction);
			//bodyinterface.SetRestitution(rigidbody.bodyID, rigidbody.restitution);
			//bodyinterface.SetGravityFactor(rigidbody.bodyID, rigidbody.gravityFactor);
			//bodyinterface.SetAngularVelocity(rigidbody.bodyID, rigidbody.angularVelocity);

			//mPhysicsSystem.OptimizeBroadPhase();

		}
		namespace {
			template <typename _physx_type, typename _collider_component>
			void SetGeom(_physx_type& geom, _collider_component& collider, Component::Transform const& transform) {
				if constexpr (std::is_same_v<_physx_type, physx::PxBoxGeometry>) {
					collider.scale = ToPxVec3(ToPhysicsUnits(transform.worldScale));
					geom = _physx_type{ collider.scale };
				}
				else if constexpr (std::is_same_v<_physx_type, physx::PxSphereGeometry>) {
					collider.radius = Physics::ToPhysicsUnits(std::max({ transform.worldScale.x, transform.worldScale.y, transform.worldScale.z }));
					geom = _physx_type{ collider.radius };
				}
				else if constexpr (std::is_same_v < _physx_type, physx::PxSphereGeometry>) {
					collider.radius = Physics::ToPhysicsUnits(std::max(transform.worldScale.x, transform.worldScale.z));
					collider.halfheight = Physics::ToPhysicsUnits(transform.worldScale.y);
					geom = _physx_type{ collider.radius, collider.halfheight };
				}
			}
		}
		template <typename _physx_type, typename _collider_component>
		void PhysicsSystem::AddShape(physx::PxRigidDynamic* rb, ECS::Entity const& entity, _collider_component & collider) {
			_physx_type geom{};
			physx::PxTransform xfm{};
			if (entity.HasComponent<Component::Transform>()) {
				Component::Transform const& transform = entity.GetComponent<Component::Transform>();
				//box shape, this will be a box collider
				SetGeom(geom, collider, transform);
				xfm = physx::PxTransform(ToPxVec3(transform.worldPos), ToPxQuat(transform.rotation));
					
			}
			else {
				//geom = physx::PxBoxGeometry(physx::PxVec3{1});
				xfm = physx::PxTransform(collider.positionOffset);
			}

			physx::PxShape* shape { mPhysics->createShape(geom, *mMaterial) };
			rb->setGlobalPose(xfm);
			shape->setLocalPose({ collider.positionOffset, collider.rotationOffset });
			rb->attachShape(*shape);

		}
		template <typename _physx_type, typename _collider_component>
		void PhysicsSystem::AddNewCollider(physx::PxRigidDynamic*& rb, ECS::Entity const& entity, _collider_component& collider) {
			Component::Transform transform = entity.GetComponent<Component::Transform>();
			if ((glm::abs(transform.worldScale.x) + glm::abs(transform.worldScale.y) + glm::abs(transform.worldScale.z)) <= glm::epsilon<float>()) {
				transform.worldScale = { 1,1,1 }; //temp fix
			}
			_physx_type geom{};
			physx::PxTransform xfm{ ToPxVec3(transform.worldPos)};
			SetGeom(geom, collider, transform);
			rb = physx::PxCreateDynamic(
				*mPhysics,
				xfm,
				geom,
				*mMaterial, 10.f); //default mass will be 10 lmao material is default also
			//ugly syntax to get the shape 
			//assumes that there is only one shape (there should only ever be one starting out)
			physx::PxShape* shape;
			rb->getShapes(&shape, 1);
			shape->setLocalPose({ collider.positionOffset, collider.rotationOffset });

			mScene->addActor(*rb);
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		}
		template <typename _physx_type, typename _collider_component>
		_collider_component& PhysicsSystem::AddCollider(ECS::Entity entity, _collider_component collider) {
			//check to prevent additional shap adding
			//if (entity.HasComponent<Component::Collider>()) return;
			physx::PxRigidDynamic* rb{};
			if (entity.HasComponent<Component::RigidBody>()) {
				auto const& rigidbody{ entity.GetComponent<Component::RigidBody>() };
				auto rbiter{ mRigidBodyIDs.find(rigidbody.bodyID) };
				if (rbiter != mRigidBodyIDs.end()) {
					rb = rbiter->second;

				}

				AddShape<_physx_type>(rb, entity, collider);
			}
			else if (entity.HasComponent<Component::Transform>()) { // this is a given
				AddNewCollider<_physx_type>(rb, entity, collider);
				mRigidBodyIDs.emplace(rb, rb);
			}
			else {
				throw std::runtime_error{"cannot have no transform or rigidbody components!!"};
			}
			collider.bodyID = reinterpret_cast<void*>(rb);
			return entity.EmplaceComponent<_collider_component>(collider);
		}
		Component::BoxCollider& PhysicsSystem::AddBoxCollider(ECS::Entity entity, Component::BoxCollider collider)
		{
			return AddCollider<physx::PxBoxGeometry>(entity, collider);
		}
		Component::SphereCollider& PhysicsSystem::AddSphereCollider(ECS::Entity entity, Component::SphereCollider collider)
		{
			return AddCollider<physx::PxSphereGeometry>(entity, collider);
		}
		Component::CapsuleCollider& PhysicsSystem::AddCapsuleCollider(ECS::Entity entity, Component::CapsuleCollider collider)
		{
			return AddCollider<physx::PxCapsuleGeometry>(entity, collider);
		}
		void PhysicsSystem::ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var)
		{
			auto const& rb{ entity.GetComponent<Component::RigidBody>() };
			auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
			if (rbiter != mRigidBodyIDs.end()) {
				physx::PxRigidDynamic* rbptr{ rbiter->second };
				if (entity.HasComponent<Component::BoxCollider>()) {
					physx::PxShape* shape;
					physx::PxMaterial* material;
					rbptr->getShapes(&shape, 1);// assuming that all the rigidbodies only have one shape
					shape->getMaterials(&material, 1);
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


				switch (var) {
				case Component::RigidBodyVars::MASS: {
					rbptr->setMass(rb.mass);
				}break;
				case Component::RigidBodyVars::GRAVITY_FACTOR: {
					//nothing here
				}break;
				case Component::RigidBodyVars::VELOCITY: {
					rbptr->setLinearVelocity(rb.velocity);
				}break;
				case Component::RigidBodyVars::ANGULAR_VELOCITY: {
					//todo add smth here
				}break;
				case Component::RigidBodyVars::MOTION: {
					rbptr->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, (bool)rb.motionType);
				}break;
				}
			}
			//AddRigidBody(ECS::Entity{});
		}

		void PhysicsSystem::ChangeBoxColliderVar(ECS::Entity entity, Component::ColliderVars var)
		{

		}

		void PhysicsSystem::ChangeSphereColliderVar(ECS::Entity entity, Component::ColliderVars var)
		{
		}

		void PhysicsSystem::ChangeCapsuleColliderVar(ECS::Entity entity, Component::ColliderVars var)
		{
		}

		void PhysicsSystem::Debug(float dt) {

			//JPH::BodyManager::DrawSettings drawSettings;
			//drawSettings.mDrawShape = true;         // Draw collision shapes
			//drawSettings.mDrawShapeWireframe = true;
			//drawSettings.mDrawBoundingBox = false;  // Disable bounding box drawing

			//// Draw debug visuals
			//mPhysicsSystem.DrawBodies(drawSettings, JPH::DebugRenderer::sInstance);
			//Graphics::Renderer::SubmitTriangle({ 10, 0, 0 }, { -10,0,0 }, { 0,0,10 }, { 1,1,1,1 });
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
		}

	}
}
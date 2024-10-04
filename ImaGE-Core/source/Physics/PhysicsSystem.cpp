#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include <Core/Components/Components.h>
#include "Core/EntityManager.h"
#include "Core/Entity.h"

#include "Physics/PhysicsHelpers.h"
namespace IGE {
	namespace Physics {
		const float gGravity{ -9.81f };
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
		//void PhysicsSystem::InitAllocator()
		//{
		//	JPH::RegisterDefaultAllocator();
		//	JPH::Trace = TraceImpl;
		//	JPH::Factory::sInstance = new JPH::Factory();
		//	JPH::DebugRenderer::sInstance = new PhysicsDebugRenderer();
		//	JPH::RegisterTypes();
		//}
		//void PhysicsSystem::Init(){
		//	mPhysicsSystem.Init(
		//		cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
		//		mBroadPhaseLayerInterface, mObjectVsBroadphaseLayerFilter, mObjectVsObjectLayerFilter
		//	);
		//	mPhysicsSystem.SetBodyActivationListener(&mBodyActivationListener);
		//	mPhysicsSystem.SetContactListener(&mContactListener);
		//	mPhysicsSystem.SetGravity(JPH::Vec3(0.f, -9.81f, 0.f));
		//}

		void PhysicsSystem::Update(float dt) {

			//mPhysicsSystem.Update(gDeltaTime, 1, &mTempAllocator, &mJobSystem);
				// Simulate one time step (1/60 second)
			mScene->simulate(1.0f / 60.0f);

			// Wait for the simulation to complete
			mScene->fetchResults(true);
			auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::RigidBody, Component::Transform>() };
			for (auto entity : rbsystem) {
				auto& xfm{ rbsystem.get<Component::Transform>(entity) };
				auto const& rb{ rbsystem.get<Component::RigidBody>(entity) };
				auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
				if (rbiter != mRigidBodyIDs.end()) {
					physx::PxRigidDynamic* pxrigidbody{ mRigidBodyIDs.at(rb.bodyID) };
					//apply gravity
					if (rb.motionType == Component::RigidBody::MotionType::DYNAMIC) {
						float grav{ gGravity * rb.gravityFactor * rb.mass };
						pxrigidbody->addForce(physx::PxVec3(0.f, grav, 0.f));
					}
					//update pos
					xfm.worldPos = ToGLMVec3(pxrigidbody->getGlobalPose().p);
				}
				//JPH::BodyInterface& bodyInterface { mPhysicsSystem.GetBodyInterface() };
				//xfm.worldPos = ToGLMVec3(bodyInterface.GetPosition(rb.bodyID));
				//xfm.worldPos = ToGLMVec3(bodyInterface.(rb.bodyID));
			}

		}

		Component::RigidBody& PhysicsSystem::AddRigidBody(ECS::Entity entity) {
			//if (entity.HasComponent<Component::RigidBody>()) return;
			Component::RigidBody rigidbody{};
			//auto& bodyinterface = mPhysicsSystem.GetBodyInterface();

				// Half extents (1 unit per side)
				//physx::PxRigidDynamic* rb = PxCreateDynamic(
				//	*mPhysics, 
				//	physx::PxTransform(ToPxVec3(transform.worldPos)),
				//	physx::PxBoxGeometry(ToPxVec3(transform.worldScale)),
				//	*mMaterial, rigidbody.mass); // Mass = 10.0f
			physx::PxRigidDynamic* rb { }; // Mass = 10.0f
			if (entity.HasComponent<Component::Collider>()) {
				auto const& collider{ entity.GetComponent<Component::Collider>() };

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
			return entity.EmplaceComponent<Component::RigidBody>(rigidbody);

			//set 
			//bodyinterface.SetFriction(rigidbody.bodyID, rigidbody.friction);
			//bodyinterface.SetRestitution(rigidbody.bodyID, rigidbody.restitution);
			//bodyinterface.SetGravityFactor(rigidbody.bodyID, rigidbody.gravityFactor);
			//bodyinterface.SetAngularVelocity(rigidbody.bodyID, rigidbody.angularVelocity);

			//mPhysicsSystem.OptimizeBroadPhase();

		}

		Component::Collider& PhysicsSystem::AddCollider(ECS::Entity entity)
		{
			//check to prevent additional shap adding
			//if (entity.HasComponent<Component::Collider>()) return;
			Component::Collider collider{};
			physx::PxRigidDynamic* rb{};
			if (entity.HasComponent<Component::RigidBody>()) {
				auto const& rigidbody{ entity.GetComponent<Component::RigidBody>() };
				auto rbiter{ mRigidBodyIDs.find(rigidbody.bodyID) };
				if (rbiter != mRigidBodyIDs.end()) {
					rb = rbiter->second;

				}
				physx::PxBoxGeometry boxgeom{};
				physx::PxTransform xfm{};
				if (entity.HasComponent<Component::Transform>()) {
					Component::Transform const& transform = entity.GetComponent<Component::Transform>();
					//box shape, this will be a box collider
					boxgeom = physx::PxBoxGeometry{ ToPxVec3(ToPhysicsUnits(transform.worldScale)) };
					xfm = physx::PxTransform(ToPxVec3(transform.worldPos) + collider.positionOffset);
				}
				else {
					boxgeom = physx::PxBoxGeometry(physx::PxVec3{1});
					xfm = physx::PxTransform(collider.positionOffset);
				}

				physx::PxShape* boxshape { mPhysics->createShape(boxgeom, *mMaterial) };

				rb->attachShape(*boxshape);
				rb->setGlobalPose(xfm);
			}
			else if (entity.HasComponent<Component::Transform>()) { // this is a given
				Component::Transform transform = entity.GetComponent<Component::Transform>();
				if ((transform.worldScale.x + transform.worldScale.y + transform.worldScale.z) <= glm::epsilon<float>()) {
					transform.worldScale = { 1,1,1 }; //temp fix
				}
				rb = physx::PxCreateDynamic(
					*mPhysics,
					physx::PxTransform(ToPxVec3(transform.worldPos) + collider.positionOffset),
					physx::PxBoxGeometry(collider.scale = ToPxVec3(ToPhysicsUnits(transform.worldScale))),
					*mMaterial, 10.f); //default mass will be 10 lmao material is default also
				mScene->addActor(*rb);
				rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
				mRigidBodyIDs.emplace(rb, rb);
			}
			else {
				throw std::runtime_error{"cannot have no transform or rigidbody components!!"};
			}
			collider.bodyID = reinterpret_cast<void*>(rb);
			return entity.EmplaceComponent<Component::Collider>(collider);
		}
		void PhysicsSystem::ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var)
		{
			auto const& rb{ entity.GetComponent<Component::RigidBody>() };
			auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
			if (rbiter != mRigidBodyIDs.end()) {
				physx::PxRigidDynamic* rbptr{ rbiter->second };
				if (entity.HasComponent<Component::Collider>()) {
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
		void PhysicsSystem::ChangeColliderShape(ECS::Entity entity)
		{
		}

		void PhysicsSystem::ChangeColliderVar(ECS::Entity entity, Component::ColliderVars var)
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
#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include "Core/Component/Components.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Graphics/Renderer.h"
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
			sceneDesc.gravity = physx::PxVec3(0.0f, 0.f , 0.0f); // Gravity pointing downward

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

		void PhysicsSystem::Update(float dt){

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
					float grav{ gGravity * rb.gravityFactor * rb.mass };
					pxrigidbody->addForce(physx::PxVec3(0.f, grav, 0.f));
					//update pos
					xfm.worldPos = ToGLMVec3(pxrigidbody->getGlobalPose().p);
				}
				//JPH::BodyInterface& bodyInterface { mPhysicsSystem.GetBodyInterface() };
				//xfm.worldPos = ToGLMVec3(bodyInterface.GetPosition(rb.bodyID));
				//xfm.worldPos = ToGLMVec3(bodyInterface.(rb.bodyID));
			}

		}

		void PhysicsSystem::AddRigidBody(ECS::Entity entity) {
			Component::RigidBody rigidbody{};
			//auto& bodyinterface = mPhysicsSystem.GetBodyInterface();
			if (entity.HasComponent<Component::Transform>()) {
				Component::Transform const& transform = entity.GetComponent<Component::Transform>();
				// Half extents (1 unit per side)
				physx::PxRigidDynamic* rb = PxCreateDynamic(
					*mPhysics, 
					physx::PxTransform(ToPxVec3(transform.worldPos)),
					physx::PxBoxGeometry(ToPxVec3(transform.worldScale)),
					*mMaterial, rigidbody.mass); // Mass = 10.0f
				physx::PxShape* shape;
				physx::PxMaterial* material;
				rb->getShapes(&shape, 1);// assuming that all the rigidbodies only have one shape
				shape->getMaterials(&material, 1);
				material->setDynamicFriction(rigidbody.dynamicFriction);
				material->setStaticFriction(rigidbody.staticFriction);
				material->setRestitution(rigidbody.restitution);
				rb->setLinearVelocity(rigidbody.velocity);
				rb->setLinearDamping(rigidbody.linearDamping);
				mScene->addActor(*rb);
				rigidbody.bodyID = reinterpret_cast<void*>(rb);
				mRigidBodyIDs.emplace(rigidbody.bodyID, rb);
				entity.EmplaceComponent<Component::RigidBody>(rigidbody);
			}
			//set 
			//bodyinterface.SetFriction(rigidbody.bodyID, rigidbody.friction);
			//bodyinterface.SetRestitution(rigidbody.bodyID, rigidbody.restitution);
			//bodyinterface.SetGravityFactor(rigidbody.bodyID, rigidbody.gravityFactor);
			//bodyinterface.SetAngularVelocity(rigidbody.bodyID, rigidbody.angularVelocity);
			
			//mPhysicsSystem.OptimizeBroadPhase();
			
		}

		void PhysicsSystem::ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var)
		{
			auto const& rb{ entity.GetComponent<Component::RigidBody>() };
			auto rbiter{ mRigidBodyIDs.find(rb.bodyID) };
			if (rbiter != mRigidBodyIDs.end()) {
				physx::PxRigidDynamic* rbptr{ rbiter->second };
				physx::PxShape* shape;
				physx::PxMaterial* material;
				rbptr->getShapes(&shape, 1);// assuming that all the rigidbodies only have one shape
				shape->getMaterials(&material, 1);

				switch (var) {
				case Component::RigidBodyVars::MASS: {
					rbptr->setMass(rb.mass);
				}break;
				case Component::RigidBodyVars::STATIC_FRICTION: {
					material->setStaticFriction(rb.staticFriction);
				}break;
				case Component::RigidBodyVars::DYNAMIC_FRICTION: {
					material->setDynamicFriction(rb.dynamicFriction);
				}break;
				case Component::RigidBodyVars::RESTITUTION: {
					material->setRestitution(rb.restitution);
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
					//todo add smth here
				}break;
				}
			}
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
#include <pch.h>
#include "Physics/PhysicsSystem.h"
#include <Core/Components/Components.h>
#include <Core/Systems/LayerSystem/LayerSystem.h>
#include "Core/EntityManager.h"
#include "Events/EventManager.h"
#include "Core/Entity.h"
#include "Scenes/SceneManager.h"
#include "Physics/PhysicsHelpers.h"
#include "Physics/PhysicsEventData.h"
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
			mRigidBodyIDs{}, mRigidBodyToEntity{}, mEventManager{ new PhysicsEventManager{mRigidBodyIDs, mRigidBodyToEntity} }
			//mTempAllocator{ 10 * 1024 * 1024 },
			//mJobSystem{ cMaxPhysicsJobs, cMaxPhysicsBarriers, static_cast<int>(thread::hardware_concurrency() - 1) }
		{
			SUBSCRIBE_CLASS_FUNC(Events::EventType::REMOVE_COMPONENT, &PhysicsSystem::HandleRemoveComponent, this);
			SUBSCRIBE_CLASS_FUNC(Events::EventType::REMOVE_ENTITY, &PhysicsSystem::HandleRemoveEntity, this);

			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			// Create the scene (simulation world)
			physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, 0.f, 0.0f); // Gravity pointing downward

			// Use default CPU dispatcher
			physx::PxDefaultCpuDispatcher* dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = dispatcher;

			if (std::shared_ptr<Systems::LayerSystem> layerSys =
				Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>().lock()) {
				sceneDesc.filterShader = LayerFilterShaderWrapper;
			}
			else {
				sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			}
			mScene = mPhysics->createScene(sceneDesc);

			// register callbacks for events
			mScene->setSimulationEventCallback(mEventManager);

			mEventManager->AddListener(PHYSICS_METHOD_LISTENER(PhysicsEventID::CONTACT, PhysicsSystem::OnContactSampleListener));
			mEventManager->AddListener(PHYSICS_METHOD_LISTENER(PhysicsEventID::TRIGGER, PhysicsSystem::OnTriggerSampleListener));
		}

		void PhysicsSystem::Update() {
			if (Scenes::SceneManager::GetInstance().GetSceneState() != Scenes::SceneState::PLAYING) {
				auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Transform>() };
				for (auto entity : rbsystem) {
					auto& xfm{ rbsystem.get<Component::Transform>(entity) };
					ECS::Entity e{entity};
					if (HasAnyComponent<Component::RigidBody, Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(e)) {
						physx::PxRigidDynamic* pxrb{};
						auto rbiter{ mRigidBodyIDs.find(nullptr) };
						if (e.HasComponent<Component::RigidBody>())
							rbiter = mRigidBodyIDs.find(e.GetComponent<Component::RigidBody>().bodyID);
						else if (e.HasComponent<Component::BoxCollider>())
							rbiter = mRigidBodyIDs.find(e.GetComponent<Component::BoxCollider>().bodyID);
						else if (e.HasComponent<Component::SphereCollider>())
							rbiter = mRigidBodyIDs.find(e.GetComponent<Component::SphereCollider>().bodyID);
						else if (e.HasComponent<Component::CapsuleCollider>())
							rbiter = mRigidBodyIDs.find(e.GetComponent<Component::CapsuleCollider>().bodyID);

						if (rbiter != mRigidBodyIDs.end()) {
							pxrb = rbiter->second;

							if (!ECS::Entity{ entity }.IsActive()) {
								if (mInactiveActors.find(pxrb) == mInactiveActors.end()) {
									mScene->removeActor(*pxrb);
									mInactiveActors.insert(pxrb);
								}
								continue;
							}
							else {
								if (mInactiveActors.find(pxrb) != mInactiveActors.end()) {
									mScene->addActor(*pxrb);
									mInactiveActors.erase(pxrb);
								}
							}
						}
						else throw std::runtime_error{ std::string("there is no rigidbody ") };

						if (pxrb->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
							pxrb->setKinematicTarget(physx::PxTransform{ToPxVec3(xfm.worldPos), ToPxQuat(xfm.worldRot)});
						else
							pxrb->setGlobalPose(physx::PxTransform{ToPxVec3(xfm.worldPos), ToPxQuat(xfm.worldRot)});
					}

					//JPH::BodyInterface& bodyInterface { mPhysicsSystem.GetBodyInterface() };
					//xfm.worldPos = ToGLMVec3(bodyInterface.GetPosition(rb.bodyID));
					//xfm.worldPos = ToGLMVec3(bodyInterface.(rb.bodyID));
				}
			}
			else {
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
						
						if (!ECS::Entity{ entity }.IsActive()) {
							if (mInactiveActors.find(pxrigidbody) == mInactiveActors.end()) {
								mScene->removeActor(*pxrigidbody);
								mInactiveActors.insert(pxrigidbody);
							}
							continue;
						}

						else {
							if (mInactiveActors.find(pxrigidbody) != mInactiveActors.end()) {
								mScene->addActor(*pxrigidbody);
								mInactiveActors.erase(pxrigidbody);
							}
						}
						
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
					}
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
					physx::PxTransform(ToPxVec3(transform.worldPos)));
				mScene->addActor(*rb);
			}
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, (bool)rigidbody.motionType);
			rb->setLinearVelocity(rigidbody.velocity);
			rb->setLinearDamping(rigidbody.linearDamping);

			rigidbody.bodyID = reinterpret_cast<void*>(rb);
			//mRigidBodyIDs.emplace(rigidbody.bodyID, rb);
			RegisterRB(rigidbody.bodyID, rb, entity);
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
				else if constexpr (std::is_same_v < _physx_type, physx::PxCapsuleGeometry>) {
					collider.radius = Physics::ToPhysicsUnits(std::max(transform.worldScale.x, transform.worldScale.z));
					collider.halfheight = Physics::ToPhysicsUnits(transform.worldScale.y);
					geom = _physx_type{ collider.radius, collider.halfheight };
				}
			}
		}

		template <typename _physx_type, typename _collider_component>
		void PhysicsSystem::AddShape(physx::PxRigidDynamic* rb, ECS::Entity const& entity, _collider_component& collider) {
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

			physx::PxShape* shape { CreateShape(geom, collider, entity) };//mPhysics->createShape(geom, *mMaterial, true) };
			rb->setGlobalPose(xfm);
			shape->setLocalPose({ collider.positionOffset, collider.rotationOffset });
			collider.idx = rb->getNbShapes();
			rb->attachShape(*shape);

		}
		template <typename _physx_type, typename _collider_component>
		void PhysicsSystem::AddNewCollider(physx::PxRigidDynamic*& rb, ECS::Entity const& entity, _collider_component& collider) {
			Component::Transform transform = entity.GetComponent<Component::Transform>();
			if ((glm::abs(transform.worldScale.x) + glm::abs(transform.worldScale.y) + glm::abs(transform.worldScale.z)) <= glm::epsilon<float>()) {
				transform.worldScale = { 1,1,1 }; //temp fix
			}
			_physx_type geom{};
			physx::PxTransform xfm(ToPxVec3(transform.worldPos), ToPxQuat(transform.rotation));
			SetGeom(geom, collider, transform);
			//rb = physx::PxCreateDynamic(
			//	*mPhysics,
			//	xfm,
			//	geom,
			//	*mMaterial, 10.f); //default mass will be 10 lmao material is default also
			rb = mPhysics->createRigidDynamic(xfm);
			//ugly syntax to get the shape 
			//assumes that there is only one shape (there should only ever be one starting out)
			physx::PxShape* shape { CreateShape(geom, collider, entity) };//mPhysics->createShape(geom, *mMaterial, true) };
			shape->setLocalPose({ collider.positionOffset, collider.rotationOffset });
			collider.idx = rb->getNbShapes();
			rb->attachShape(*shape);
			mScene->addActor(*rb);
			collider.idx = 0;
			rb->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		}
		template <typename _physx_type, typename _collider_component>
		_collider_component& PhysicsSystem::AddCollider(ECS::Entity entity, _collider_component collider) {
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

				AddShape<_physx_type>(rb, entity, collider);
			}
			else if (entity.HasComponent<Component::Transform>()) { // this is a given
				AddNewCollider<_physx_type>(rb, entity, collider);
				//mRigidBodyIDs.emplace(rb, rb);
				RegisterRB(reinterpret_cast<void*>(rb), rb, entity);
			}
			else {
				throw std::runtime_error{"cannot have no transform or rigidbody components!!"};
			}
			collider.bodyID = reinterpret_cast<void*>(rb);
			return entity.EmplaceComponent<_collider_component>(collider);
		}

		template<typename _physx_type, typename _collider_component>
		physx::PxShape* PhysicsSystem::CreateShape(_physx_type const& geom, _collider_component const& collider, ECS::Entity entity)
		{
			physx::PxShape* shapeptr{ mPhysics->createShape(geom, *mMaterial, true) };

			if (std::shared_ptr<Systems::LayerSystem> layerSys =
				Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>().lock()) {
				layerSys->SetupShapeFilterData(&shapeptr, entity);
			}
			SetColliderAsSensor(shapeptr, collider.sensor);
			return shapeptr;
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
				case Component::RigidBodyVars::ANGULAR_VELOCITY: {
					//todo add smth here
					return;
				}break;
				case Component::RigidBodyVars::MOTION: {
					rbptr->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, (bool)rb.motionType);
					return;
				}break;
				}
				if (entity.HasComponent<Component::BoxCollider>()) {
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
			auto const& collider{ entity.GetComponent<Component::BoxCollider>() };
			auto shapeptr { GetShapePtr<Component::BoxCollider>(collider) };
			shapeptr->setLocalPose(physx::PxTransform(collider.positionOffset, collider.rotationOffset));
			shapeptr->setGeometry(physx::PxBoxGeometry{ collider.scale });
			SetColliderAsSensor(shapeptr, collider.sensor);
		}

		void PhysicsSystem::ChangeSphereColliderVar(ECS::Entity entity)
		{
			auto const& collider{ entity.GetComponent<Component::SphereCollider>() };
			auto shapeptr{ GetShapePtr<Component::SphereCollider>(collider) };
			auto shapeid{ shapeptr->getGeometryType()};
			shapeptr->setLocalPose(physx::PxTransform(collider.positionOffset, collider.rotationOffset));
			shapeptr->setGeometry(physx::PxSphereGeometry{ collider.radius });
			SetColliderAsSensor(shapeptr, collider.sensor);
		}

		void PhysicsSystem::ChangeCapsuleColliderVar(ECS::Entity entity)
		{
			auto const& collider{ entity.GetComponent<Component::CapsuleCollider>() };
			auto shapeptr{ GetShapePtr<Component::CapsuleCollider>(collider) };
			shapeptr->setLocalPose(physx::PxTransform(collider.positionOffset, collider.rotationOffset));
			shapeptr->setGeometry(physx::PxCapsuleGeometry{ collider.radius, collider.halfheight });
			SetColliderAsSensor(shapeptr, collider.sensor);
		}

		void PhysicsSystem::Debug(float dt) {


		}

		void PhysicsSystem::ClearSystem()
		{
			for (auto rbpair : mRigidBodyIDs) {
				auto rb{ rbpair.second };
				mScene->removeActor(*rb);
				rb->release();
			}
			mRigidBodyIDs.clear();
			mRigidBodyToEntity.clear();
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

			if (numShapes == 1) {
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
			if (e->mType == rttr::type::get<Component::RigidBody>()) {
				RemoveRigidBody(e->mEntity);
			} 
			else if (e->mType == rttr::type::get<Component::BoxCollider>()) {
				RemoveCollider<physx::PxBoxGeometry, Component::BoxCollider>(e->mEntity, physx::PxGeometryType::Enum::eBOX);
			}
			else if (e->mType == rttr::type::get<Component::SphereCollider>()) {
				RemoveCollider<physx::PxSphereGeometry, Component::SphereCollider>(e->mEntity, physx::PxGeometryType::Enum::eSPHERE);
			}
			else if (e->mType == rttr::type::get<Component::CapsuleCollider>()) {
				RemoveCollider<physx::PxCapsuleGeometry, Component::CapsuleCollider>(e->mEntity, physx::PxGeometryType::Enum::eCAPSULE);
			}
		}
		EVENT_CALLBACK_DEF(PhysicsSystem, HandleRemoveEntity) {
			auto e{ CAST_TO_EVENT(Events::RemoveEntityEvent) };
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
			physx::PxPairFlag::eNOTIFY_CONTACT_POINTS |
			physx::PxPairFlag::eTRIGGER_DEFAULT;
		if (auto layerSys = Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>().lock()) {
			return layerSys->LayerFilterShader(
				attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize);
		}

		return physx::PxFilterFlag::eDEFAULT;
	}
}
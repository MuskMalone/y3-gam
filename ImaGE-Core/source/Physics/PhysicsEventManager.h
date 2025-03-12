#pragma once
#include <functional>
#include <vector>
#include <Core/GUID.h>
#include <Physics/PhysicsEvent.h>
#include <Core/Entity.h>
///for member functions
#define PHYSICS_METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)

///for regular functions
#define PHYSICS_FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

#define PHYSICS_EVENT_LISTENER_DECL(name) void name(IGE::Physics::PhysicsEvent const& e);
#define PHYSICS_EVENT_LISTENER_IMPL(name) void name(IGE::Physics::PhysicsEvent const& e)
namespace IGE {
	namespace Physics {
		enum class TriggerResult : int {
			NONE = 0, LOST, FOUND, PERSISTS
		};
		enum class ContactResult {
			NONE = 0, FOUND, LOST, PERSISTS
		};
		class PhysicsEventManager : public physx::PxSimulationEventCallback {
		public:
			PhysicsEventManager(
				std::unordered_map<void*, physx::PxRigidDynamic*>& rbid,
				std::unordered_map<void*, ECS::Entity>& rbToEntity,
				std::unordered_map<void*, std::unordered_map<void*, int>>& triggerpairs,
				std::unordered_map<void*, std::unordered_map<void*, std::vector<physx::PxContactPairPoint>>>& contactpairs
			) : mRigidBodyIDs{ rbid }, mRigidBodyToEntity{ rbToEntity }, mOnTriggerPairs{ triggerpairs }, mOnContactPairs{ contactpairs } {
			}
		private:
			PhysicsEventManager() = delete;

		public:
			/**
			\brief This is called when a breakable constraint breaks.

			\note The user should not release the constraint shader inside this call!

			\note No event will get reported if the constraint breaks but gets deleted while the time step is still being simulated.

			\param[in] constraints - The constraints which have been broken.
			\param[in] count       - The number of constraints

			@see PxConstraint PxConstraintDesc.linearBreakForce PxConstraintDesc.angularBreakForce
			*/
			virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;

			/**
			\brief This is called with the actors which have just been woken up.

			\note Only supported by rigid bodies yet.
			\note Only called on actors for which the PxActorFlag eSEND_SLEEP_NOTIFIES has been set.
			\note Only the latest sleep state transition happening between fetchResults() of the previous frame and fetchResults() of the current frame
			will get reported. For example, let us assume actor A is awake, then A->putToSleep() gets called, then later A->wakeUp() gets called.
			At the next simulate/fetchResults() step only an onWake() event will get triggered because that was the last transition.
			\note If an actor gets newly added to a scene with properties such that it is awake and the sleep state does not get changed by
			the user or simulation, then an onWake() event will get sent at the next simulate/fetchResults() step.

			\param[in] actors - The actors which just woke up.
			\param[in] count  - The number of actors

			@see PxScene.setSimulationEventCallback() PxSceneDesc.simulationEventCallback PxActorFlag PxActor.setActorFlag()
			*/
			virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override;

			/**
			\brief This is called with the actors which have just been put to sleep.

			\note Only supported by rigid bodies yet.
			\note Only called on actors for which the PxActorFlag eSEND_SLEEP_NOTIFIES has been set.
			\note Only the latest sleep state transition happening between fetchResults() of the previous frame and fetchResults() of the current frame
			will get reported. For example, let us assume actor A is asleep, then A->wakeUp() gets called, then later A->putToSleep() gets called.
			At the next simulate/fetchResults() step only an onSleep() event will get triggered because that was the last transition (assuming the simulation
			does not wake the actor up).
			\note If an actor gets newly added to a scene with properties such that it is asleep and the sleep state does not get changed by
			the user or simulation, then an onSleep() event will get sent at the next simulate/fetchResults() step.

			\param[in] actors - The actors which have just been put to sleep.
			\param[in] count  - The number of actors

			@see PxScene.setSimulationEventCallback() PxSceneDesc.simulationEventCallback PxActorFlag PxActor.setActorFlag()
			*/
			virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override;

			/**
			\brief This is called when certain contact events occur.

			The method will be called for a pair of actors if one of the colliding shape pairs requested contact notification.
			You request which events are reported using the filter shader/callback mechanism (see #PxSimulationFilterShader,
			#PxSimulationFilterCallback, #PxPairFlag).

			Do not keep references to the passed objects, as they will be
			invalid after this function returns.

			\param[in] pairHeader Information on the two actors whose shapes triggered a contact report.
			\param[in] pairs The contact pairs of two actors for which contact reports have been requested. See #PxContactPair.
			\param[in] nbPairs The number of provided contact pairs.

			@see PxScene.setSimulationEventCallback() PxSceneDesc.simulationEventCallback PxContactPair PxPairFlag PxSimulationFilterShader PxSimulationFilterCallback
			*/
			virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

			/**
			\brief This is called with the current trigger pair events.

			Shapes which have been marked as triggers using PxShapeFlag::eTRIGGER_SHAPE will send events
			according to the pair flag specification in the filter shader (see #PxPairFlag, #PxSimulationFilterShader).

			\note Trigger shapes will no longer send notification events for interactions with other trigger shapes.

			\param[in] pairs - The trigger pair events.
			\param[in] count - The number of trigger pair events.

			@see PxScene.setSimulationEventCallback() PxSceneDesc.simulationEventCallback PxPairFlag PxSimulationFilterShader PxShapeFlag PxShape.setFlag()
			*/
			virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

			/**
			\brief Provides early access to the new pose of moving rigid bodies.

			When this call occurs, rigid bodies having the #PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW
			flag set, were moved by the simulation and their new poses can be accessed through the provided buffers.

			\note The provided buffers are valid and can be read until the next call to #PxScene::simulate() or #PxScene::collide().

			\note Buffered user changes to the rigid body pose will not yet be reflected in the provided data. More important,
			the provided data might contain bodies that have been deleted while the simulation was running. It is the user's
			responsibility to detect and avoid dereferencing such bodies.

			\note This callback gets triggered while the simulation is running. If the provided rigid body references are used to
			read properties of the object, then the callback has to guarantee no other thread is writing to the same body at the same
			time.

			\note The code in this callback should be lightweight as it can block the simulation, that is, the
			#PxScene::fetchResults() call.

			\param[in] bodyBuffer The rigid bodies that moved and requested early pose reporting.
			\param[in] poseBuffer The integrated rigid body poses of the bodies listed in bodyBuffer.
			\param[in] count The number of entries in the provided buffers.

			@see PxScene.setSimulationEventCallback() PxSceneDesc.simulationEventCallback PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW
			*/
			virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;
		public:/*  _________________________________________________________________________ */
			/*! AddListener

			@param eventId The ID of the event to listen for.
			@param listener The function to be called when the event is triggered.

			@return none.

			Adds a listener function for a specific event ID. When an event with the
			specified ID is sent, the listener function will be called.
			*/
			void AddListener(PhysicsEventID eventId, std::function<void(PhysicsEvent const&)> const& listener);

				/*  _________________________________________________________________________ */
			/*! SendEvent

			@param event The event to be sent.

			@return none.

			Sends the specified event to all registered listeners for that event type.
			Each listener function is called with the event as an argument.
			*/

			void SendEvent(PhysicsEvent const& event);

			/*  _________________________________________________________________________ */
			/*! SendEvent

			@param eventId The ID of the event to be sent.

			@return none.

			Creates and sends an event with the specified ID to all registered listeners
			for that event type. Each listener function is called with the event as an argument.
			*/
			void SendEvent(PhysicsEventID eventId);

		private:
			std::unordered_map<PhysicsEventID, std::list<std::function<void(PhysicsEvent const&)>>> listeners;

			//cant mod, just observe
			std::unordered_map<void*, physx::PxRigidDynamic*> const& mRigidBodyIDs;
			std::unordered_map<void*, ECS::Entity> const& mRigidBodyToEntity;
			std::unordered_map<void*, std::unordered_map<void*, int>>& mOnTriggerPairs;
			std::unordered_map<void*, std::unordered_map<void*, std::vector<physx::PxContactPairPoint>>>& mOnContactPairs;

		};
	}
}
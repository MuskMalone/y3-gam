#include <pch.h>
#include "PhysicsEventManager.h"
#include "Physics/PhysicsEventData.h"
namespace IGE {
	namespace Physics {
		
		void PhysicsEventManager::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) 
		{
			PhysicsEvent e{ PhysicsEventID::CONSTRAINT_BREAK };
			SendEvent(e);
		}
		void PhysicsEventManager::onWake(physx::PxActor** actors, physx::PxU32 count)
		{
			PhysicsEvent e{ PhysicsEventID::WAKE };
			SendEvent(e);
		}
		void PhysicsEventManager::onSleep(physx::PxActor** actors, physx::PxU32 count)
		{
			PhysicsEvent e{ PhysicsEventID::SLEEP };
			SendEvent(e);
		}
		void PhysicsEventManager::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
		{
			if (pairHeader.actors[0]->getType() == physx::PxActorType::eRIGID_DYNAMIC && pairHeader.actors[1]->getType() == physx::PxActorType::eRIGID_DYNAMIC) {
				auto firstactor{ reinterpret_cast<physx::PxRigidDynamic*>(pairHeader.actors[0]) };
				auto secondactor{ reinterpret_cast<physx::PxRigidDynamic*>(pairHeader.actors[1]) };
				//assume that all the actors are rigiddynamics
				std::pair<ECS::Entity, ECS::Entity> entitypair{
					mRigidBodyToEntity.at(reinterpret_cast<void*>(firstactor)),
					mRigidBodyToEntity.at(reinterpret_cast<void*>(secondactor))
				};

				std::vector<physx::PxContactPairPoint> cp;
				for (unsigned i{}; i < nbPairs; ++i) {
					physx::PxContactPair const& contactPair{ pairs[i] };
					physx::PxContactPairPoint contactPoints[64]; // i dont think there are so many contact points available anyways
					auto numContacts{ contactPair.extractContacts(contactPoints, 64) };
					for (unsigned cpidx{}; cpidx < numContacts; ++cpidx) {
						cp.emplace_back(contactPoints[cpidx]);
					}
				}
				//std::cout << "contact\n";

				PhysicsEvent e{ PhysicsEventID::CONTACT };
				e.SetParam(EventKey::EventContact::ENTITY_PAIR, entitypair);
				e.SetParam(EventKey::EventContact::CONTACT_POINTS, cp);
				SendEvent(e);
			}
		}
		void PhysicsEventManager::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
		{
			for (unsigned i{}; i < count; ++i)
			{
				const physx::PxTriggerPair& triggerPair = pairs[i];
				physx::PxActor* actor1 = triggerPair.triggerActor;
				physx::PxActor* actor2 = triggerPair.otherActor;
				if (actor1->getType() == physx::PxActorType::eRIGID_DYNAMIC && actor2->getType() == physx::PxActorType::eRIGID_DYNAMIC) {
					auto triggeractor{ reinterpret_cast<physx::PxRigidDynamic*>(actor1) };
					auto otheractor{ reinterpret_cast<physx::PxRigidDynamic*>(actor2) };
					auto voidtrigger{ reinterpret_cast<void*>(triggeractor) };
					auto voidother{ reinterpret_cast<void*>(otheractor) };
					//determine the trigger result
					TriggerResult result{TriggerResult::NONE};
					switch (pairs[i].status) {
					case physx::PxPairFlag::eNOTIFY_TOUCH_FOUND: 
						result = TriggerResult::FOUND; 
						break;
					case physx::PxPairFlag::eNOTIFY_TOUCH_LOST:
						result = TriggerResult::LOST; 
						break;
					}
					//add to the trigger pair
					mOnTriggerPairs[voidtrigger].emplace(voidother, (int)result);

					PhysicsEvent e{ PhysicsEventID::TRIGGER };
					e.SetParam(EventKey::EventTrigger::TRIGGER_ENTITY, mRigidBodyToEntity.at(voidtrigger));
					e.SetParam(EventKey::EventTrigger::OTHER_ENTITY, mRigidBodyToEntity.at(voidother));
					SendEvent(e);
				}
			}
		}
		void PhysicsEventManager::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
		{
		}

		/*! AddListener

		@param eventId The ID of the event to listen for.
		@param listener The function to be called when the event is triggered.

		@return none.

		Adds a listener function for a specific event ID. When an event with the
		specified ID is sent, the listener function will be called.
		*/

		void PhysicsEventManager::AddListener(PhysicsEventID eventId, std::function<void(PhysicsEvent const&)> const& listener)
		{
			listeners[eventId].push_back(listener);
		}
		void PhysicsEventManager::SendEvent(PhysicsEvent const& event)
		{
			auto type{ event.GetType() };

			for (auto const& listener : listeners[type])
			{
				listener(event);
			}
		}

		/*  _________________________________________________________________________ */
		/*! SendEvent

		@param eventId The ID of the event to be sent.

		@return none.

		Creates and sends an event with the specified ID to all registered listeners
		for that event type. Each listener function is called with the event as an argument.
		*/

		void PhysicsEventManager::SendEvent(PhysicsEventID eventId)
		{
			PhysicsEvent event(eventId);

			for (auto const& listener : listeners[eventId])
			{
				listener(event);
			}
		}
	}
}
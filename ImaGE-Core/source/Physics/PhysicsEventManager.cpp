#include <pch.h>
#include "PhysicsEventManager.h"

namespace IGE {
	namespace Physics {
		
		void PhysicsEventManager::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) 
		{
			PhysicsEvent e{ PhysicsEventID::CONSTRAINT_BREAK };

			SendEvent(e);
			std::cout << "constraint break\n";
		}
		void PhysicsEventManager::onWake(physx::PxActor** actors, physx::PxU32 count)
		{
			PhysicsEvent e{ PhysicsEventID::WAKE };
			SendEvent(e);
			std::cout << "wake\n";
		}
		void PhysicsEventManager::onSleep(physx::PxActor** actors, physx::PxU32 count)
		{
			PhysicsEvent e{ PhysicsEventID::SLEEP };
			SendEvent(e);
			std::cout << "sleep\n";
		}
		void PhysicsEventManager::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
		{
			std::cout << "contact\n";
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
					if (contactPair.events & physx::PxPairFlag::eCONTACT_DEFAULT) {
						physx::PxContactPairPoint contactPoints[16]; // i dont think there are so many contact points available anyways
						auto numContacts{ contactPair.extractContacts(contactPoints, sizeof(contactPoints) / sizeof(physx::PxContactPairPoint)) };
						for (unsigned cpidx{}; cpidx < numContacts; ++cpidx) {
							cp.emplace_back(contactPoints[cpidx]);
						}
					}
				}
				std::cout << "contact\n";
			}
		}
		void PhysicsEventManager::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
		{
			std::cout << "trigger\n";

		}
		void PhysicsEventManager::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
		{
			std::cout << "advance\n";
		}

		/*! AddListener

		@param eventId The ID of the event to listen for.
		@param listener The function to be called when the event is triggered.

		@return none.

		Adds a listener function for a specific event ID. When an event with the
		specified ID is sent, the listener function will be called.
		*/

		inline void PhysicsEventManager::AddListener(PhysicsEventID eventId, std::function<void(PhysicsEvent const&)> const& listener)
		{
			listeners[eventId].push_back(listener);
		}
		inline void PhysicsEventManager::SendEvent(PhysicsEvent const& event)
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

		inline void PhysicsEventManager::SendEvent(PhysicsEventID eventId)
		{
			PhysicsEvent event(eventId);

			for (auto const& listener : listeners[eventId])
			{
				listener(event);
			}
		}
	}
}
#include <pch.h>
#include "PhysicsFilter.h"

namespace IGE {
    namespace Physics {

        physx::PxFilterFlags PhysicsFilter::pairFound(
            physx::PxU32 pairID,
            physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, const physx::PxActor* a0, const physx::PxShape* s0,
            physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, const physx::PxActor* a1, const physx::PxShape* s1,
            physx::PxPairFlags& pairFlags) {
            // Example: Enable contact notification for this pair
            pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;     // Notify when contact is first found
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;  // Notify when contact persists
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;      // Notify when contact is lost

            // Optionally, add more logic to filter certain pairs
            // e.g., ignore specific object types or enable special processing

            return physx::PxFilterFlag::eDEFAULT;
        }

        // Called when the simulation determines that the pair is no longer relevant

        void PhysicsFilter::pairLost(physx::PxU32 pairID, physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, bool objectRemoved) {
            // Handle the case where the pair is lost, if needed (optional)
            std::cout << "Pair lost between two objects" << std::endl;
        }

        // Called when the filter status of a pair changes (rarely used)

        bool PhysicsFilter::statusChange(physx::PxU32& pairID, physx::PxPairFlags& pairFlags, physx::PxFilterFlags& filterFlags) {
            return false;
        }
    }
}
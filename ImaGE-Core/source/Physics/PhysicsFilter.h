#pragma once
#include "PxPhysicsAPI.h"
namespace IGE {
    namespace Physics {
        class PhysicsFilter :
            public physx::PxSimulationFilterCallback
        {
        public:
            // Called when a pair of objects is about to be processed by the simulation
            virtual physx::PxFilterFlags pairFound(physx::PxU32 pairID,
                physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, const physx::PxActor* a0, const physx::PxShape* s0,
                physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, const physx::PxActor* a1, const physx::PxShape* s1,
                physx::PxPairFlags& pairFlags) override;

            // Called when the simulation determines that the pair is no longer relevant
            virtual void pairLost(physx::PxU32 pairID,
                physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                bool objectRemoved) override;

            // Called when the filter status of a pair changes (rarely used)
            virtual bool statusChange(physx::PxU32& pairID, physx::PxPairFlags& pairFlags, physx::PxFilterFlags& filterFlags) override;
        };
    }
}

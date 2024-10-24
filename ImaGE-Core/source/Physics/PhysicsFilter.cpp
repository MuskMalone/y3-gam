#include <pch.h>
#include "PhysicsFilter.h"

namespace IGE {
    namespace Physics {
        physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {
            // Enable default collision and notify on touch
            pairFlags |= 
                physx::PxPairFlag::eCONTACT_DEFAULT | 
                physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | 
                physx::PxPairFlag::eNOTIFY_TOUCH_LOST | 
                physx::PxPairFlag::eNOTIFY_CONTACT_POINTS | 
                physx::PxPairFlag::eTRIGGER_DEFAULT;
            return physx::PxFilterFlag::eDEFAULT;
        }
    }
}
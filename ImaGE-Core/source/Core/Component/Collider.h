#pragma once

namespace Component {
    enum class ColliderVars {
        SCALE, POS
    };

    struct Collider {
        inline void Clear() noexcept {
            scale = physx::PxVec3();
            positionOffset = physx::PxVec3();
        }

        physx::PxVec3 scale{0, 0, 0};
        physx::PxVec3 positionOffset{0, 0, 0};

        void* bodyID;
    };
}
#pragma once

namespace Component {
    enum class ColliderVars {   
        SCALE, POS, ROT
    };
    
    struct Collider {
      enum class ColliderShape {

      };
      inline void Clear() noexcept { 
        scale = physx::PxVec3();
        positionOffset = physx::PxVec3();
        rotationOffset = physx::PxVec3();
      }

      physx::PxVec3 scale{0,0,0};
      physx::PxVec3 positionOffset{0,0,0};
      physx::PxVec3 rotationOffset{0,0,0};

      void* bodyID;
      ColliderShape type;
    };
}

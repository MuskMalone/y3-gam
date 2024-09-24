#pragma once
#include <Jolt/Physics/Body/Body.h>

namespace Component {
    enum class ColliderVars {   
        SCALE, POS, ROT
    };
    struct Collider {

      inline void Clear() noexcept { 
        scale = JPH::Vec3();
        positionOffset = JPH::Vec3();
        rotationOffset = JPH::Vec3();
      }

      JPH::Vec3 scale;
      JPH::Vec3 positionOffset;
      JPH::Vec3 rotationOffset;

      JPH::BodyID bodyID;
      JPH::EShapeSubType type;
    };
}

#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vector.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

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

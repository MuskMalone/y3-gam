#pragma once
#include <pch.h>
namespace Component {
    struct Collider {
        JPH::Vec3 scale;
        JPH::Vec3 position;
        JPH::Quat rotation;

        JPH::EShapeSubType type;
        JPH::BodyID bodyID;
    };
}
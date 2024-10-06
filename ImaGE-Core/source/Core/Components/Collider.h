/*!*********************************************************************
\file   Collider.h
\author
\date   6-October-2024
\brief  Definition of Collider component used by the physics system

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <PxPhysicsAPI.h>

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

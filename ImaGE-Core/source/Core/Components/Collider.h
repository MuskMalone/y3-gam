/*!*********************************************************************
\file   Collider.h
\author
\date   6-October-2024
\brief  Definition of Collider component used by the physics system

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <PxPhysicsAPI.h>
#include <rttr/rttr_enable.h>

namespace Component {
  struct ColliderAbstract {
  public:
      inline void Clear() noexcept {
          positionOffset = { 0, 0, 0 };
          rotationOffset = {physx::PxIdentity()};
          sensor = false;
      }
      physx::PxVec3 positionOffset{0, 0, 0};
      physx::PxQuat rotationOffset{physx::PxIdentity()};
      
      void* bodyID; // not serialized. decided at runtime
      char idx; // not serialized. max value 3

      bool sensor{ false };

      RTTR_ENABLE()
  };

  struct BoxCollider : public ColliderAbstract{
  public:
    inline void Clear() noexcept {
        ColliderAbstract::Clear();
      scale = physx::PxVec3();
    }
    physx::PxVec3 scale{0, 0, 0};

    RTTR_ENABLE(ColliderAbstract)
  };

  struct SphereCollider : public ColliderAbstract {
  public:
      inline void Clear() noexcept {
          ColliderAbstract::Clear();
          radius = 0.f;
      }
      float radius;

      RTTR_ENABLE(ColliderAbstract)
  };

  struct CapsuleCollider : public ColliderAbstract {
  public:
      inline void Clear() noexcept {
          ColliderAbstract::Clear();
          radius = 0.f;
          halfheight = 0.f;
      }
      float radius;
      float halfheight;

      RTTR_ENABLE(ColliderAbstract)
  };

}

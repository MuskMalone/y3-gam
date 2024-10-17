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
  enum class ColliderVars {
     SCALE, POS, ROT
  };
  struct ColliderAbstract {
      RTTR_ENABLE()
  public:
      inline void Clear() noexcept {
          positionOffset = {};
          rotationOffset = {};
          sensor = false;
      }
      physx::PxVec3 positionOffset{0, 0, 0};
      physx::PxQuat rotationOffset{};
      void* bodyID;
      bool sensor{ false };
  };
  struct BoxCollider : public ColliderAbstract{
      RTTR_ENABLE(ColliderAbstract)
  public:
    inline void Clear() noexcept {
        ColliderAbstract::Clear();
      scale = physx::PxVec3();
    }
    physx::PxVec3 scale{0, 0, 0};

  };
  struct SphereCollider : public ColliderAbstract {
      RTTR_ENABLE(ColliderAbstract)
  public:
      inline void Clear() noexcept {
          ColliderAbstract::Clear();
          radius = 0.f;
      }
      float radius;
  };
  struct CapsuleCollider : public ColliderAbstract {
      RTTR_ENABLE(ColliderAbstract)
  public:
      inline void Clear() noexcept {
          ColliderAbstract::Clear();
          radius = 0.f;
          halfheight = 0.f;
      }
      float radius;
      float halfheight;
  };

}

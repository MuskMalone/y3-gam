/*!*********************************************************************
\file   PreTransformSystem.h
\author chengen.lau\@digipen.edu
\date   03-October-2024
\brief  Computes the world transform of entities based on their local
        transform.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/Systems/System.h>
#include <Core/Entity.h>

namespace Systems {
  class PreTransformSystem : public System {
  public:
    PreTransformSystem(const char* name) : System(name) {}

    /*!*********************************************************************
    \brief
      Update function of the system
    ************************************************************************/
    void Update() override;

  private:
    /*!*********************************************************************
    \brief
      Recursively computes the local transform of an entity based on its
      world transform
    \param entity
     The current entity
    ************************************************************************/
    void UpdateLocalTransform(ECS::Entity entity, bool parentModified);
  };
} // namespace Systems
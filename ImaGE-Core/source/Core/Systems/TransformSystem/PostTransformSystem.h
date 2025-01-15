/*!*********************************************************************
\file   PostTransformSystem.h
\author chengen.lau\@digipen.edu
\date   03-October-2024
\brief  Updates the local / world transform of all entities based on
        what was modified in the engine.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/Systems/System.h>
#include <Core/Entity.h>

namespace Systems {
  class PostTransformSystem : public System {
  public:
    PostTransformSystem(const char* name) : System(name) {}

    /*!*********************************************************************
    \brief
      Update function of the system
    ************************************************************************/
    void Update() override;

  private:

    /*!*********************************************************************
    \brief
      Recursively computes the world transform of an entity based on its
      local transform
    \param entity
     The current entity
    ************************************************************************/
    void UpdateWorldTransform(ECS::Entity entity, bool parentModified);
  };
} // namespace Systems
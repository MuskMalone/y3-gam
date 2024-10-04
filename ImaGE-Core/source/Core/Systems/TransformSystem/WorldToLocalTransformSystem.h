/*!*********************************************************************
\file   WorldToLocalTransformSystem.h
\author chengen.lau\@digipen.edu
\date   03-October-2024
\brief  Computes the local transform of entities based on their world
        transform. Should only run after world transform is updated.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/Systems/System.h>
#include <Core/Entity.h>

namespace Systems {
  class WorldToLocalTransformSystem : public System {
  public:
    WorldToLocalTransformSystem(const char* name) : System(name) {}

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
    void UpdateWorldToLocal(ECS::Entity entity);
  };
} // namespace Systems
/*!*********************************************************************
\file   LocalToWorldTransformSystem.h
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
  class LocalToWorldTransformSystem : public System {
  public:
    LocalToWorldTransformSystem(const char* name) : System(name) {}

    void Update() override;

  private:
    void UpdateLocalToWorld(ECS::Entity entity);
  };
} // namespace Systems
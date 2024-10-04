/*!*********************************************************************
\file   TransformHelpers.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Contains the declarations of helper functions dealing with
        updating transforms of entities.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/Components/Transform.h>
#include <Core/Entity.h>

namespace TransformHelpers {

  /*!*********************************************************************
  \brief
    Updates the transform of an entity along with all its children
    based on a new parent
  \param entity
    The entity with the new parent
  ************************************************************************/
  void UpdateTransformToNewParent(ECS::Entity entity);

} // namespace TransformHelpers

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
#include <glm/glm.hpp>

namespace TransformHelpers {

  void UpdateWorldTransform(ECS::Entity entity);

  /*!*********************************************************************
  \brief
    Helper function to recursively update all world transforms of a
    hierarchy
  \param entity
    The child entity to update
  ************************************************************************/
  void UpdateWorldTransformRecursive(ECS::Entity entity);

  /*!*********************************************************************
  \brief
    Sets the world position of an entity
  \param newPos
    The new world position
  ************************************************************************/
  void SetEntityWorldPos(ECS::Entity entity, glm::vec3 const& newPos);

  /*!*********************************************************************
  \brief
    Sets the world scale of an entity
  \param newScale
    The new world scale
  ************************************************************************/
  void SetEntityWorldScale(ECS::Entity entity, glm::vec3 const& newScale);

  /*!*********************************************************************
  \brief
    Updates the transform of an entity along with all its children
    based on a new parent
  \param entity
    The entity with the new parent
  ************************************************************************/
  void UpdateTransformToNewParent(ECS::Entity entity);

} // namespace TransformHelpers

/*!*********************************************************************
\file       VariantEntity.h
\author     chengen.lau\@digipen.edu
\date       15-September-2024
\brief
    Definition for struct VariantEntity, which stores deserialized
    data for entities. Used for loading of scenes, creating and
    duplicating of entities, and in editing-related engine tools.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <rttr/type.h>
#include <Core/EntityManager.h>

namespace Reflection
{

  // struct encapsulating deserialized data of an entity
  // this is pre-creation of the entity where components
  // are stored as rttr::variants in a vector
  struct VariantEntity
  {
    using EntityID = ECS::EntityManager::EntityID;

    VariantEntity() = default;
    VariantEntity(std::string const& name, EntityID parent = entt::null, bool active = true)
      : mName{ name }, mPrefab{}, mComponents {}, mChildEntities{}, mParent{ parent }, mIsActive{ active } {}

    std::string mName, mPrefab;
    std::vector<rttr::variant> mComponents;
    std::vector<EntityID> mChildEntities;
    EntityID mParent = entt::null;
    bool mIsActive;
  };

} // namespace Reflection

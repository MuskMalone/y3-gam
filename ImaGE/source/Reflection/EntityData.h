/*!*********************************************************************
\file       EntityData.h
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
#include <Core/Component/PrefabOverrides.h>
#include <optional>
#include <glm/glm.hpp>

namespace Reflection
{

  // struct encapsulating deserialized data of an entity
  // this is pre-creation of the entity where components
  // are stored as rttr::variants in a vector
  struct VariantEntity
  {
    using EntityID = ECS::EntityManager::EntityID;

    VariantEntity(EntityID id, EntityID parent = entt::null, bool active = true)
      : mComponents {}, mChildEntities{}, mID{ id }, mParent{ parent }, mIsActive{ active } {}

    std::vector<rttr::variant> mComponents;
    std::vector<EntityID> mChildEntities;
    EntityID mID, mParent;
    bool mIsActive;
  };

  // add ID and ParentID
  struct PrefabInst
  {
    PrefabInst() : mPosition{}, mOverrides{} {}

    std::optional<glm::vec3> mPosition; // only for root entity
    Component::PrefabOverrides mOverrides;
  };

} // namespace Reflection

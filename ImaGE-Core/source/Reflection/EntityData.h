/*!*********************************************************************
\file       EntityData.h
\author     chengen.lau\@digipen.edu
\date       15-September-2024
\brief
    Definition for structs used to store deserialized
    data for entities. Used for loading of scenes, creating and
    duplicating of entities, and in editing-related engine tools.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <rttr/type.h>
#include <Core/EntityManager.h>
#include <Serialization/PfbOverridesData.h>
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
    using EntityID = ECS::EntityManager::EntityID;

    PrefabInst() : mOverrides{}, mChildren{}, mName{}, mPosition {}, mId{ entt::null }, mParent{ entt::null } {}
    PrefabInst(EntityID id, const char* name, EntityID parent = entt::null)
      : mOverrides{}, mChildren{}, mName{ name }, mPosition {}, mId{ id }, mParent{ parent } {}

    Serialization::PfbOverridesData mOverrides;
    std::vector<EntityID> mChildren;
    std::string mName;
    std::optional<glm::vec3> mPosition; // only for root entity
    EntityID mId, mParent;
  };

} // namespace Reflection


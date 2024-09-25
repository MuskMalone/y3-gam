/*!*********************************************************************
\file   Prefab.h
\author chengen.lau\@digipen.edu
\date   16-September-2024
\brief  
  Contains the definition of the struct encapsulating deserialized
  prefab data. It is used during creation of entities from prefabs and
  when editing prefabs in the prefab editor. The implementation makes
  use of RTTR library to store components as rttr::variant objects.
  Each prefab also allows for multiple layers of components.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <vector>
#include <string>
#include <rttr/type.h>
#include <Core/Entity.h>

namespace Prefabs
{
  using PrefabVersion = unsigned;

  // struct encapsulating deserialized data of a prefab's child components
  // this should only be instantiated for use with a Prefab
  struct PrefabSubData
  {
    using SubDataId = unsigned;

    PrefabSubData();
    PrefabSubData(SubDataId id, SubDataId parent = BasePrefabId);

    /*!*********************************************************************
    \brief
      Adds a component held by an rttr::variant object into the SubData's
      vector of components
    \param comp
      The component to add
    ************************************************************************/
    void AddComponent(rttr::variant const& comp) { mComponents.emplace_back(comp); }

    /*!*********************************************************************
    \brief
      Adds a component held by an rttr::variant object into the SubData's
      vector of components
    \param comp
      The component to add
    ************************************************************************/
    void AddComponent(rttr::variant&& comp) { mComponents.emplace_back(std::move(comp)); }

    /*!*********************************************************************
    \brief
      Constructs an entity with the data in the current object. The entity
      will be created with its components and is automatically added to
      the ECS.
      This function does not handle the mapping of parent-child relations.
    \return
      The ID of the created entity
    ************************************************************************/
    ECS::Entity Construct() const;

    std::vector<rttr::variant> mComponents;
    SubDataId mId, mParent;
    bool mIsActive;

    // id of the first layer of the prefab
    static SubDataId const BasePrefabId = 0;
  };
  // struct encapsulating deserialized prefab data
  // components are stored in an std::vector of rttr::variants
  struct Prefab
  {
    struct EntityMappings;  // forward declaration; definition below

    Prefab() = default;
    Prefab(std::string name);

    /*!*********************************************************************
    \brief
      Checks if the current prefab contains any child/nested components
    \return
      True if child components exist and false otherwise
    ************************************************************************/
    inline bool HasChildComponents() const noexcept { return !mObjects.empty(); }

    /*!*********************************************************************
    \brief
      Constructs an entity with the data in the current Prefab.
      The entity will be created along with it's relevant hierarchy and
      is automatically added to the ECS.
    \return
      The ID of the created entity
    ************************************************************************/
    std::pair<ECS::Entity, EntityMappings> Construct() const;

    /*!*********************************************************************
    \brief
      Creates a sub-object of given the set of entity IDs from the ECS and
      adds it to mObjects vector. Calling this function does not require
      the 2nd argument to be used. This function is recursively called 
      downwards for each child in the set until an entity without
      children is encountered.
    \param children
      The list of child entities of the current entity
    \param parent
      The SubDataId of the parent (current) object.
      This is defaulted to the BasePrefabId and should not be used
      externally.
    ************************************************************************/
    void CreateSubData(std::vector<ECS::Entity> const& children, PrefabSubData::SubDataId parent = PrefabSubData::BasePrefabId);

    /*!*********************************************************************
    \brief
      Resets the Prefab object 
    ************************************************************************/
    void Clear() noexcept;

    std::string mName;
    std::vector<PrefabSubData> mObjects;
    std::vector<rttr::variant> mComponents;
    bool mIsActive;
  };

  // stores information pertaining to each prefab instance
  struct Prefab::EntityMappings
  {
    EntityMappings() : mPrefab{}, mObjToEntity{} {}
    EntityMappings(std::string prefab) : mPrefab{ std::move(prefab) }, mObjToEntity{} {}

    /*!*********************************************************************
    \brief
      Checks through the mappings of the current prefab based on the scene
      and removes any obsolete entries corresponding to destroyed entities.
    ************************************************************************/
    void Validate();

    std::string mPrefab;
    std::unordered_map<PrefabSubData::SubDataId, ECS::EntityManager::EntityID> mObjToEntity;
  };
}
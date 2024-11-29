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
#include <unordered_map>
#include <unordered_set>
#include <rttr/type.h>
#include <Core/EntityManager.h>
#include <Asset/SmartPointer.h>

// forward declaration
namespace Reflection { class ObjectFactory; struct PrefabInst; }

namespace Prefabs
{
  using SubDataId = unsigned;

  // struct encapsulating deserialized data of a prefab's child components
  // this should only be instantiated for use with a Prefab
  struct PrefabSubData
  {
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
    \param guid
      The guid of the prefab
    \param createInst
      Whether to add the prefab overrides component
    \return
      The ID of the created entity
    ************************************************************************/
    ECS::Entity Construct(IGE::Assets::GUID guid, bool createInst = true) const;

    std::vector<rttr::variant> mComponents;
    SubDataId mId, mParent;

    // id of the first layer of the prefab
    static constexpr SubDataId BasePrefabId = 0;
  };
  // struct encapsulating deserialized prefab data
  // components are stored in an std::vector of rttr::variants
  struct Prefab
  {
    struct EntityMappings;
    using SubObjectComponentMap = std::unordered_map<SubDataId, std::vector<rttr::variant>>;

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
    \param guid
      The guid of the prefab
    \param
      The position to construct the prefab at
    \return
      The ID of the created entity
    ************************************************************************/
    ECS::Entity Construct(IGE::Assets::GUID guid, glm::vec3 const& pos = {}) const;

    /*!*********************************************************************
    \brief
      Constructs an entity with the data in the current Prefab.
      The entity will be created along with it's relevant hierarchy and
      is automatically added to the ECS. This overload stores the mappings
      of each entity to its subdata and returns it as an EntityMappings
      object.
    \param
      The position to construct the prefab at
    \return
      The ID of the created entity and the entity mappings as a pair
    ************************************************************************/
    std::pair<ECS::Entity, EntityMappings> ConstructAndMap(glm::vec3 const& pos = {}) const;

    /*!*********************************************************************
    \brief
      Fills up a prefab instance with its components along with any missing
      sub-objects
    \param guid
      The guid of the prefab
    \param idToEntity
      The map of ids to entities
    ************************************************************************/
    void FillPrefabInstance(IGE::Assets::GUID guid, std::unordered_map<Prefabs::SubDataId, ECS::Entity>& idToEntity) const;

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
    void CreateSubData(IGE::Assets::GUID guid, std::vector<ECS::Entity> const& children,
      SubDataId parent = PrefabSubData::BasePrefabId);
    void CreateFixedSubData(std::vector<ECS::Entity> const& children, EntityMappings& mappings, SubDataId parent = PrefabSubData::BasePrefabId);

    /*!*********************************************************************
    \brief
      Resets the Prefab object 
    ************************************************************************/
    void Clear() noexcept;

    std::string mName;
    std::vector<PrefabSubData> mObjects;
    std::vector<rttr::variant> mComponents;
  };

  struct Prefab::EntityMappings {

    void Reserve(size_t size) { mappings.reserve(size); idDict.reserve(size); }

    void Insert(ECS::EntityManager::EntityID entityId, SubDataId subDataId) {
      mappings.emplace(entityId, subDataId);
      idDict.emplace(subDataId);
    }

    inline SubDataId Get(ECS::EntityManager::EntityID const& id) { return mappings[id]; }
    inline SubDataId Get(ECS::EntityManager::EntityID const& id) const { return mappings.at(id); }
    inline bool Contains(ECS::EntityManager::EntityID const& id) const { return mappings.contains(id); }
    inline bool Contains(SubDataId id) const { return idDict.contains(id); }
    inline size_t Size() const { return idDict.size(); }
    inline bool Empty() const noexcept { return idDict.empty(); }

    std::unordered_map<ECS::EntityManager::EntityID, SubDataId> mappings;
    std::unordered_set<SubDataId> idDict;
  };
}
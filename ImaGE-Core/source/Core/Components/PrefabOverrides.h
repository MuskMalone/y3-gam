/*!*********************************************************************
\file   PrefabOverrides.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of the PrefabOverrides component, which is a hidden
        component for prefab instances that keeps track of what
        components have been modified over the original prefab.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <unordered_set>
#include <rttr/variant.h>
#include <Prefabs/Prefab.h>
#include <Asset/SmartPointer.h>

namespace Component {
  struct PrefabOverrides
  {
    PrefabOverrides() = default;
    PrefabOverrides(IGE::Assets::GUID _guid, Prefabs::SubDataId id = Prefabs::PrefabSubData::BasePrefabId) :
      modifiedComponents{}, removedComponents{}, guid{ _guid }, subDataId{ id } {}

    inline bool IsDefault() const noexcept { return /*propOverrides.empty() &&*/ modifiedComponents.empty() && removedComponents.empty(); }
    inline bool IsRoot() const noexcept { return subDataId == Prefabs::PrefabSubData::BasePrefabId; }

    /*!*********************************************************************
    \brief
      Checks if a component exists in the list of modified/added components
    \return
      True if the component has been overriden and false otherwise
    ************************************************************************/
    template <typename T>
    inline bool IsComponentModified() const { return modifiedComponents.contains(rttr::type::get<T>()); }

    /*!*********************************************************************
    \brief
      Checks if a component exists in the list of removed components
    \param
      The type of the component
    \return
      True if the component has been removed and false otherwise
    ************************************************************************/
    inline bool IsComponentModified(rttr::type const& type) const { return modifiedComponents.contains(type); }
    
    /*!*********************************************************************
    \brief
      Checks if a component exists in the list of removed components
    \return
      True if the component has been removed and false otherwise
    ************************************************************************/
    template <typename T>
    inline bool IsComponentRemoved() const { return removedComponents.contains(rttr::type::get<T>()); }

    /*!*********************************************************************
    \brief
      Checks if a component exists in the list of removed components
    \param
      The type of the component
    \param
      The type of the component
    \return
      True if the component has been removed and false otherwise
    ************************************************************************/
    inline bool IsComponentRemoved(rttr::type const& type) const { return removedComponents.contains(type); }

    /*!*********************************************************************
    \brief
      Adds a component to the modified list
    \param comp
      The component to add
    ************************************************************************/
    template <typename T>
    void AddComponentOverride() {
      rttr::type compType{ rttr::type::get<T>() };
      removedComponents.erase(compType);
      modifiedComponents.emplace(std::move(compType));
    }
    void AddComponentOverride(rttr::type const& compType) {
      removedComponents.erase(compType);
      modifiedComponents.emplace(compType);
    }

    /*!*********************************************************************
    \brief
      Adds a component to the removed list
    \param type
      The type of the removed component
    ************************************************************************/
    template <typename T>
    void AddComponentRemoval() {
      // make sure to extract the base type if it is a wrapper/ptr
      rttr::type compType{ rttr::type::get<T>() };
      modifiedComponents.erase(compType);
      removedComponents.emplace(std::move(compType));
    }
    void AddComponentRemoval(rttr::type const& type) {
      // make sure to extract the base type if it is a wrapper/ptr
      modifiedComponents.erase(type);
      removedComponents.emplace(type);
    }

    template <typename T>
    void RemoveOverride() {
      rttr::type compType{ rttr::type::get<T>() };
      modifiedComponents.erase(compType);
      removedComponents.erase(compType);
    }
    void RemoveOverride(rttr::type const& compType) {
      modifiedComponents.erase(compType);
      removedComponents.erase(compType);
    }

    void Reset() noexcept {
      modifiedComponents.clear();
      removedComponents.clear();
    }

    std::unordered_set<rttr::type> modifiedComponents, removedComponents;
    IGE::Assets::GUID guid;
    Prefabs::SubDataId subDataId;
  };

} // namespace Component

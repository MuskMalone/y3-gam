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
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <rttr/variant.h>
#include <Prefabs/Prefab.h>
#include <Asset/IGEAssets.h>

namespace Component {
  struct PrefabOverrides
  {
    PrefabOverrides() = default;
    PrefabOverrides(IGE::Assets::GUID _guid, Prefabs::SubDataId id = Prefabs::PrefabSubData::BasePrefabId) :
      guid{ _guid }, modifiedComponents{}, removedComponents{}, subDataId{ id } {}

    inline bool IsDefault() const noexcept { return /*propOverrides.empty() &&*/ modifiedComponents.empty() && removedComponents.empty(); }
    
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
    void AddComponentModification(T const& comp) {
      rttr::type const compType{ rttr::type::get<T>() };
      removedComponents.erase(compType);
      modifiedComponents[compType] = std::make_shared<T>(comp);
    }

    /*!*********************************************************************
    \brief
      Adds a component to the removed list
    \param type
      The type of the removed component
    ************************************************************************/
    void AddComponentRemoval(rttr::type const& type) { modifiedComponents.erase(type); removedComponents.emplace(type); }

    IGE::Assets::GUID guid;
    std::unordered_map<rttr::type, rttr::variant> modifiedComponents;
    std::unordered_set<rttr::type> removedComponents;
    Prefabs::SubDataId subDataId;
  };

} // namespace Component

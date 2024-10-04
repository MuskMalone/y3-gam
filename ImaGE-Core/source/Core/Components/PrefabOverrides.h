#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <rttr/variant.h>
#include <Prefabs/Prefab.h>

namespace Component {
  struct PrefabOverrides
  {
    PrefabOverrides() = default;
    PrefabOverrides(std::string const& prefab, Prefabs::SubDataId id = Prefabs::PrefabSubData::BasePrefabId) :
      prefabName{ prefab }, modifiedComponents{}, removedComponents{}, subDataId{ id } {}

    inline bool IsDefault() const noexcept { return /*propOverrides.empty() &&*/ modifiedComponents.empty() && removedComponents.empty(); }
    
    // checks if a component exists in the list of modified/added components
    template <typename T>
    inline bool IsComponentModified() const { return modifiedComponents.contains(rttr::type::get<T>()); }
    inline bool IsComponentModified(rttr::type const& type) const { return modifiedComponents.contains(type); }
    // checks if a component exists in the list of removed components
    template <typename T>
    inline bool IsComponentRemoved() const { return removedComponents.contains(rttr::type::get<T>()); }
    inline bool IsComponentRemoved(rttr::type const& type) const { return removedComponents.contains(type); }

    template <typename T>
    void AddComponentModification(T const& comp) {
      rttr::type const compType{ rttr::type::get<T>() };
      removedComponents.erase(compType);
      modifiedComponents[compType] = std::make_shared<T>(comp);
    }
    void AddComponentRemoval(rttr::type const& type) { modifiedComponents.erase(type); removedComponents.emplace(type); }

    std::string prefabName; // @TODO: SHOULD USE GUID WHEN ASSET MANAGER DONE
    std::unordered_map<rttr::type, rttr::variant> modifiedComponents;
    std::unordered_set<rttr::type> removedComponents;
    Prefabs::SubDataId subDataId;
  };

} // namespace Component

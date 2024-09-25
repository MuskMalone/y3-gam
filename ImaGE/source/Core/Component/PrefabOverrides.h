#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <rttr/variant.h>

namespace Component {
  /*struct PropOverride {
    PropOverride() : propName{}, propVal{} {}
    PropOverride(rttr::property const& prop) : propName{ prop.get_name().to_string() }, propVal{ prop } {}

    std::string propName;
    rttr::variant propVal;
  };*/

  struct PrefabOverrides
  {
    PrefabOverrides() = default;
    PrefabOverrides(std::string const& prefab) : prefabName{ prefab }, /*propOverrides{}, */modifiedComponents{}, removedComponents{} {}

    inline bool IsDefault() const noexcept { return /*propOverrides.empty() &&*/ modifiedComponents.empty() && removedComponents.empty(); }
    
    // checks if a component exists in the list of modified/added components
    template <typename T>
    inline bool IsComponentModified() const { return modifiedComponents.contains(rttr::type::get<T>()); }
    inline bool IsComponentModified(rttr::type const& type) const { return modifiedComponents.contains(type); }
    // checks if a component exists in the list of removed components
    template <typename T>
    inline bool IsComponentRemoved() const { return removedComponents.contains(rttr::type::get<T>()); }
    inline bool IsComponentRemoved(rttr::type const& type) const { return removedComponents.contains(type); }

    void AddComponentModification(rttr::type const& type) { removedComponents.erase(type); modifiedComponents.emplace(type); }
    void AddComponentRemoval(rttr::type const& type) { modifiedComponents.erase(type); removedComponents.emplace(type); }
    /*inline void AddProperty(rttr::property const& prop) { propOverrides.emplace(std::piecewise_construct,
      std::forward_as_tuple(prop.get_declaring_type()), std::forward_as_tuple(prop)); }*/

    std::string prefabName; // @TODO: SHOULD USE GUID WHEN ASSET MANAGER DONE
    //std::unordered_map<rttr::type, PropOverride> propOverrides; // <component, property>
    std::unordered_set<rttr::type> modifiedComponents, removedComponents;
  };

} // namespace Component

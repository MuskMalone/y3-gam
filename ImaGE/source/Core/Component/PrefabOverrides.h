#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <rttr/variant.h>

namespace Component {
  struct PropOverride {
    PropOverride() : propName{}, propVal{} {}
    PropOverride(rttr::property const& prop) : propName{ prop.get_name().to_string() }, propVal{ prop } {}

    std::string propName;
    rttr::variant propVal;
  };

  struct PrefabOverrides
  {
    PrefabOverrides() = default;
    PrefabOverrides(std::string const& prefab) : prefabName{ prefab }, propOverrides{}, addedComponents{}, removedComponents{} {}

    inline bool IsDefault() const noexcept { return propOverrides.empty() && addedComponents.empty() && removedComponents.empty(); }
    inline void AddProperty(rttr::property const& prop) { propOverrides.emplace(std::piecewise_construct,
      std::forward_as_tuple(prop.get_declaring_type()), std::forward_as_tuple(prop)); }
    inline void AddComponentAddition(rttr::type const& type) { addedComponents.emplace_back(type); }
    inline void AddComponentRemoval(rttr::type const& type) { removedComponents.emplace_back(type); }

    struct TypeWrapper {
      TypeWrapper(rttr::type const& _type = rttr::type::get<void>()) : type{ _type } {}
      rttr::type type;
    };

    std::string prefabName; // @TODO: SHOULD USE GUID WHEN ASSET MANAGER DONE
    std::unordered_map<rttr::type, PropOverride> propOverrides; // <component, property>
    std::vector<TypeWrapper> addedComponents, removedComponents;
  };

} // namespace Component


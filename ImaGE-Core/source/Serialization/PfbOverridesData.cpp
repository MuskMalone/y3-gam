#include <pch.h>
#include "PfbOverridesData.h"
#include <Reflection/ObjectFactory.h>

namespace Serialization {

  PfbOverridesData::PfbOverridesData(Component::PrefabOverrides const& overrides, ECS::Entity entity)
    : componentData{}, removedComponents{}, guid{ overrides.guid }, subDataId{ overrides.subDataId } {
    for (rttr::type const& type : overrides.removedComponents) {
      removedComponents.emplace(type.get_name().to_string());
    }

    FillData(overrides.modifiedComponents, entity);
  }

  void PfbOverridesData::FillData(std::unordered_set<rttr::type> const& components, ECS::Entity entity) {
    Reflection::ObjectFactory& of{ Reflection::ObjectFactory::GetInstance() };

    for (rttr::type const& type : components) {
      componentData.emplace(type.get_name().to_string(), of.GetEntityComponent(entity, type));
    }
  }

  Component::PrefabOverrides PfbOverridesData::ToPrefabOverrides() const {
    Component::PrefabOverrides ret{ guid, subDataId };
    ret.modifiedComponents.reserve(componentData.size());
    ret.removedComponents.reserve(removedComponents.size());

    for (auto const& iter : componentData) {
      ret.modifiedComponents.emplace(rttr::type::get_by_name(iter.first));
    }

    for (std::string const& typeStr : removedComponents) {
      ret.removedComponents.emplace(rttr::type::get_by_name(typeStr));
    }

    return ret;
  }

} // namespace Serialization

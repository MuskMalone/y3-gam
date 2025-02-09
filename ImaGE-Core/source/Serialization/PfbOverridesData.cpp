#include <pch.h>
#include "PfbOverridesData.h"
#include <Reflection/ObjectFactory.h>

namespace Serialization {

  PfbOverridesData::PfbOverridesData(Component::PrefabOverrides const& overrides, ECS::Entity entity)
    : componentData{}, removedComponents{}, guid{ overrides.guid }, subDataId{ overrides.subDataId } {
    for (rttr::type const& type : overrides.removedComponents) {
      removedComponents.emplace(type);
    }

    FillData(overrides.modifiedComponents, entity);
  }

  void PfbOverridesData::FillData(std::unordered_set<rttr::type> const& components, ECS::Entity entity) {
    Reflection::ObjectFactory& of{ Reflection::ObjectFactory::GetInstance() };

    for (rttr::type const& type : components) {
      componentData.emplace(type, of.GetEntityComponent(entity, type));
    }
  }

  Component::PrefabOverrides PfbOverridesData::ToPrefabOverrides() const {
    Component::PrefabOverrides ret{ guid, subDataId };
    ret.modifiedComponents.reserve(componentData.size());
    ret.removedComponents.reserve(removedComponents.size());

    for (auto const& iter : componentData) {
      ret.modifiedComponents.emplace(iter.first);
    }

    for (rttr::type const& type : removedComponents) {
      ret.removedComponents.emplace(type);
    }

    return ret;
  }

} // namespace Serialization

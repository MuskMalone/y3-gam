#pragma once
#include <Core/Components/PrefabOverrides.h>
#include <map>

namespace ECS { class Entity; }

namespace Serialization {
  struct PfbOverridesData {
    PfbOverridesData() = default;
    PfbOverridesData(Component::PrefabOverrides const& overrides, ECS::Entity entity);

    void FillData(std::unordered_set<rttr::type> const& components, ECS::Entity entity);
    Component::PrefabOverrides ToPrefabOverrides() const;

    // using componentName as key to keep it ordered for serialization
    std::map<std::string, rttr::variant> componentData;
    std::set<std::string> removedComponents;
    IGE::Assets::GUID guid;
    Prefabs::SubDataId subDataId;
  };
} // namespace Serialization

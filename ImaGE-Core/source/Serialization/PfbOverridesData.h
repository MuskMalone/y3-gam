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

    // keep components ordered for serialization
    std::map<rttr::type, rttr::variant> componentData;
    std::set<rttr::type> removedComponents;
    IGE::Assets::GUID guid;
    Prefabs::SubDataId subDataId;
  };
} // namespace Serialization

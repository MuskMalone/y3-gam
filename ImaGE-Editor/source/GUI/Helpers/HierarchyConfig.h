#pragma once
#include <Core/Entity.h>
#include <ImGui/imgui.h>
#include <set>

namespace GUI {
  struct HierarchyEntry {
    HierarchyEntry() : entityId{}, stackId{}, isOpen{ false } {}
    HierarchyEntry(ECS::Entity::EntityID _entityId, ImGuiID _stackId, bool open) :
      entityId{ _entityId }, stackId{ _stackId }, isOpen{ open } {}

    ECS::Entity::EntityID entityId;
    ImGuiID stackId;
    bool isOpen;

    bool operator <(HierarchyEntry const& rhs) const {
      return entityId < rhs.entityId;
    }
  };

  struct HierarchyConfig {
    std::set<HierarchyEntry> collapsedNodes;
  };
}

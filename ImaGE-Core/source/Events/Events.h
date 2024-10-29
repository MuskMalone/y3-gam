/*!*********************************************************************
\file   Events.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  Definitions of general subclasses of the Event class.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Event.h"
#include <Core/Entity.h>
#include <vector>
#include "InputEvents.h"
#include "SceneEvents.h"

namespace Events
{
  class WindowGainFocus : public Event
  {
  public:
    WindowGainFocus() : Event(EventType::WINDOW_GAIN_FOCUS) {}
    inline std::string GetName() const noexcept override { return "Window Gained Focus"; }
  };

  class WindowLoseFocus : public Event
  {
  public:
    WindowLoseFocus() : Event(EventType::WINDOW_LOSE_FOCUS) {}
    inline std::string GetName() const noexcept override { return "Window Lost Focus"; }
  };

  class WindowMinimized : public Event
  {
  public:
    WindowMinimized() : Event(EventType::WINDOW_MINIMIZED) {}
    inline std::string GetName() const noexcept override { return "Window Minimized"; }
  };

  class ToggleFullscreen : public Event
  {
  public:
    ToggleFullscreen() : Event(EventType::TOGGLE_FULLSCREEN) {}
    inline std::string GetName() const noexcept override { return "Toggle Fullscreen"; }
  };

  class ZoomInOnEntity : public Event
  {
  public:
    ZoomInOnEntity(ECS::Entity entity) : Event(EventType::ENTITY_ZOOM), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Zooming in on Entity " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
  };

  // name, path, pos = {}, mapEntity = true
  class SpawnPrefabEvent : public Event
  {
  public:
    SpawnPrefabEvent(std::string name, std::string path, glm::vec3 const& pos = {}, bool mapEntity = true) : Event(EventType::SPAWN_PREFAB),
      mName{ std::move(name) }, mPath{ std::move(path) }, mPos{ pos }, mMapEntity{ mapEntity } {}
    inline std::string GetName() const noexcept override { return "Spawn Prefab: " + mName; }

    std::string const mName, mPath;
    glm::vec3 const mPos;
    bool const mMapEntity;
  };

#ifndef IMGUI_DISABLE
  class RemoveComponentEvent : public Event
  {
  public:
    RemoveComponentEvent(ECS::Entity entity, rttr::type const& compType) : Event(EventType::REMOVE_COMPONENT), mEntity{ entity }, mType{ compType } {}
    inline std::string GetName() const noexcept override { return "Deleted Component " + mType.get_name().to_string()
      + " from entity " + std::to_string(mEntity.GetEntityID()); }

    template <typename T>
    bool IsSameType() const noexcept { return mType == rttr::type::get<T>(); }

    rttr::type const mType;
    ECS::Entity const mEntity;
  };

  class RemoveEntityEvent : public Event
  {
  public:
    RemoveEntityEvent(ECS::Entity entity) : Event(EventType::REMOVE_ENTITY), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Deleted Entity: " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
  };

  // int pathCount, const char* paths[]
  class AddFilesFromExplorerEvent : public Event
  {
  public:
    AddFilesFromExplorerEvent(int pathCount, const char* paths[]) : Event(EventType::ADD_FILES) {
      for (int i{}; i < pathCount; ++i) {
        mPaths.emplace_back(paths[i]);
      }
    }
    inline std::string GetName() const noexcept override { return "Adding " + std::to_string(mPaths.size()) + " files from file explorer"; }

    std::vector<std::string> mPaths;
  };
#endif

  class EntityLayerModified : public Event
  {
  public:
    EntityLayerModified(ECS::Entity entity, std::string oldLayer) : Event(EventType::LAYER_MODIFIED), mEntity{ entity }, mOldLayer{ oldLayer } {}
    inline std::string GetName() const noexcept override { return "Modified Layer Component of Entity: " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
    std::string mOldLayer;
  };
}

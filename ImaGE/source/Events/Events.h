/*!*********************************************************************
\file   Events.h
\author wchinkit.bryan\@digipen.edu
\date   10-September-2024
\brief  Definitions of general subclasses of the Event class.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Event.h"
#include <Core/EntityManager.h>
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

  class QuitGame : public Event
  {
  public:
    QuitGame() : Event(EventType::QUIT_GAME) {}
    inline std::string GetName() const noexcept override { return "Quit Game"; }
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
  class DeletePrefabEvent : public Event
  {
  public:
    DeletePrefabEvent(std::string name) : Event(EventType::DELETE_PREFAB), mName{ std::move(name) } {}
    inline std::string GetName() const noexcept override { return "Deleted Prefab: " + mName; }

    std::string const mName;
  };

  class RemoveEntityEvent : public Event
  {
  public:
    RemoveEntityEvent(ECS::EntityManager::EntityID id) : Event(EventType::REMOVE_ENTITY), mEntityId{ id } {}
    inline std::string GetName() const noexcept override { return "Deleted Prefab: " + static_cast<unsigned>(mEntityId); }

    ECS::EntityManager::EntityID const mEntityId;
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

  class PrefabInstancesUpdatedEvent : public Event
  {
  public:
    PrefabInstancesUpdatedEvent() : Event(EventType::PREFAB_INSTANCES_UPDATED) {}
    inline std::string GetName() const noexcept override { return "Scene Updated with Prefab Instances"; }
  };
#endif

#ifdef GAM200_EVENTS
#ifndef IMGUI_DISABLE

  class PrefabSavedEvent : public Event
  {
  public:
    PrefabSavedEvent(std::string prefab) : Event(EventType::PREFAB_SAVED), mPrefab{ std::move(prefab) } {}
    inline std::string GetName() const noexcept override { return "Prefab Saved: " + mPrefab; }

    std::string const mPrefab;
  };

  class DeleteAssetEvent : public Event
  {
  public:
    DeleteAssetEvent(Assets::AssetType type, std::string name) : Event(EventType::DELETE_ASSET), mType{ type }, mName{ std::move(name) } {}
    inline std::string GetName() const noexcept override { return "Deleted Asset: " + mName + " of type " + std::to_string(static_cast<int>(mType)); }

    Assets::AssetType const mType;
    std::string const mName;
  };

  class DeletePrefabChildEvent : public Event
  {
  public:
    DeletePrefabChildEvent(ECS::Entity entity) : Event(EventType::DELETE_PREFAB_CHILD), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Deleted entity " + std::to_string(mEntity) + " of prefab"; }

    ECS::Entity const mEntity;
  };

#endif
#endif
}

/*!*********************************************************************
\file   Events.h
\author wchinkit.bryan\@digipen.edu
\date   10-September-2024
\brief  Definitions of general subclasses of the Event class.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Event.h"

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

#ifdef GAM200_EVENTS
#ifndef IMGUI_DISABLE

  class NewSceneEvent : public Event
  {
  public:
    NewSceneEvent(std::string name) : Event(EVENT_TYPE::NEW_SCENE), m_sceneName{ name } {}
    inline std::string GetName() const noexcept override { return "New Scene " + m_sceneName + " created"; }

    std::string const m_sceneName;
  };

  class StartSceneEvent : public Event
  {
  public:
    StartSceneEvent() : Event(EVENT_TYPE::START_SCENE) {}
    inline std::string GetName() const noexcept override { return "Scene Started"; }
  };

  class StopSceneEvent : public Event
  {
  public:
    StopSceneEvent() : Event(EVENT_TYPE::STOP_SCENE) {}
    inline std::string GetName() const noexcept override { return "Scene Stopped"; }
  };

  class PrefabSavedEvent : public Event
  {
  public:
    PrefabSavedEvent(std::string prefab) : Event(EVENT_TYPE::PREFAB_SAVED), m_prefab{ std::move(prefab) } {}
    inline std::string GetName() const noexcept override { return "Prefab Saved: " + m_prefab; }

    std::string const m_prefab;
  };

  class PrefabInstancesUpdatedEvent : public Event
  {
  public:
    PrefabInstancesUpdatedEvent() : Event(EVENT_TYPE::PREFAB_INSTANCES_UPDATED) {}
    inline std::string GetName() const noexcept override { return "Scene Updated with Prefab Instances"; }
  };

  class EditPrefabEvent : public Event
  {
  public:
    EditPrefabEvent(std::string prefab, std::string path) : Event(EVENT_TYPE::EDIT_PREFAB), m_prefab{ std::move(prefab) }, m_path{ std::move(path) } {}
    inline std::string GetName() const noexcept override { return "Editing Prefab: " + m_prefab; }

    std::string const m_prefab, m_path;
  };

  class DeletePrefabEvent : public Event
  {
  public:
    DeletePrefabEvent(std::string name) : Event(EVENT_TYPE::DELETE_PREFAB), m_name{ std::move(name) } {}
    inline std::string GetName() const noexcept override { return "Deleted Prefab: " + m_name; }

    std::string const m_name;
  };

  class DeleteAssetEvent : public Event
  {
  public:
    DeleteAssetEvent(Assets::AssetType type, std::string name) : Event(EVENT_TYPE::DELETE_ASSET), m_type{ type }, m_name{ std::move(name) } {}
    inline std::string GetName() const noexcept override { return "Deleted Asset: " + m_name + " of type " + std::to_string(static_cast<int>(m_type)); }

    Assets::AssetType const m_type;
    std::string const m_name;
  };

  class DeletePrefabChildEvent : public Event
  {
  public:
    DeletePrefabChildEvent(ECS::Entity entity) : Event(EVENT_TYPE::DELETE_PREFAB_CHILD), m_entity{ entity } {}
    inline std::string GetName() const noexcept override { return "Deleted entity " + std::to_string(m_entity) + " of prefab"; }

    ECS::Entity const m_entity;
  };

#endif
#endif
}

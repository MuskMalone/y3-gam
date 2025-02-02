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
#include "AssetEvents.h"

namespace Events
{
  class WindowGainFocus : public Event
  {
  public:
    WindowGainFocus() : Event() {}
    inline std::string GetName() const noexcept override { return "Window Gained Focus"; }
  };

  class WindowLoseFocus : public Event
  {
  public:
    WindowLoseFocus() : Event() {}
    inline std::string GetName() const noexcept override { return "Window Lost Focus"; }
  };

  class WindowMinimized : public Event
  {
  public:
    WindowMinimized() : Event() {}
    inline std::string GetName() const noexcept override { return "Window Minimized"; }
  };

  class WindowResized : public Event
  {
  public:
      WindowResized(int width, int height) : Event(), mWidth{width}, mHeight{height} {}
      inline std::string GetName() const noexcept override { return "Window Resized"; }
      int const mWidth;
      int const mHeight;
  };

  class ToggleFullscreen : public Event
  {
  public:
    ToggleFullscreen() : Event() {}
    inline std::string GetName() const noexcept override { return "Toggle Fullscreen"; }
  };

  class ZoomInOnEntity : public Event
  {
  public:
    ZoomInOnEntity(ECS::Entity entity) : Event(), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Zooming in on Entity " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
  };

  // name, path, pos = {}, mapEntity = true
  class SpawnPrefabEvent : public Event
  {
  public:
    SpawnPrefabEvent(std::string name, std::string path, glm::vec3 const& pos = {}, bool mapEntity = true) : Event(),
      mName{ std::move(name) }, mPath{ std::move(path) }, mPos{ pos }, mMapEntity{ mapEntity } {}
    inline std::string GetName() const noexcept override { return "Spawn Prefab: " + mName; }

    std::string const mName, mPath;
    glm::vec3 const mPos;
    bool const mMapEntity;
  };

  class RemoveComponentEvent : public Event
  {
  public:
    RemoveComponentEvent(ECS::Entity entity, rttr::type const& compType) : Event(), mEntity{ entity }, mType{ compType } {}
    inline std::string GetName() const noexcept override { return "Deleted Component " + mType.get_name().to_string()
      + " from entity " + std::to_string(mEntity.GetEntityID()); }

    template <typename T>
    bool IsSameType() const noexcept { return mType == rttr::type::get<T>(); }

    rttr::type const mType;
    ECS::Entity const mEntity;
  };

  // entity
  class RemoveEntityEvent : public Event
  {
  public:
    RemoveEntityEvent(ECS::Entity entity) : Event(), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Deleted Entity: " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
  };

  class EntityLayerModified : public Event
  {
  public:
    EntityLayerModified(ECS::Entity entity, std::string oldLayer) : Event(), mEntity{ entity }, mOldLayer{ std::move(oldLayer) } {}
    inline std::string GetName() const noexcept override { return "Modified Layer Component of Entity: " + mEntity.GetTag(); }

    std::string const mOldLayer;
    ECS::Entity const mEntity;
  };

  // entity
  class EntityScreenPicked : public Event {
  public:
    EntityScreenPicked(ECS::Entity entity) : Event(), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Screen-picked entity " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
  };

  // entity
  class EntitySelectedInEditor : public Event {
  public:
    EntitySelectedInEditor(ECS::Entity entity) : Event(), mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Selected entity " + mEntity.GetTag(); }

    ECS::Entity const mEntity;
  };

  class EntityMouseEnter : public Event {
  public:
      EntityMouseEnter(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On mouse enter entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityMouseExit : public Event {
  public:
      EntityMouseExit(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On mouse exit entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityMouseDown : public Event {
  public:
      EntityMouseDown(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On mouse down entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityMouseUp : public Event {
  public:
      EntityMouseUp(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On mouse up entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityPointerEnter : public Event {
  public:
      EntityPointerEnter(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On pointer enter entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityPointerExit : public Event {
  public:
      EntityPointerExit(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On pointer exit entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityPointerDown : public Event {
  public:
      EntityPointerDown(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On pointer down entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class EntityPointerUp : public Event {
  public:
      EntityPointerUp(ECS::Entity entity) : Event(), mEntity{ entity } {}
      inline std::string GetName() const noexcept override { return "On pointer up entity " + mEntity.GetTag(); }

      ECS::Entity mEntity;
  };

  class SignalEvent : public Event {
  public:
    SignalEvent() : Event() {}
    inline std::string GetName() const noexcept override { return "Program terminated unexpectedly"; }
  };

  class TriggerPausedUpdate : public Event {
  public:
    TriggerPausedUpdate() : Event() {}
    inline std::string GetName() const noexcept override { return "Triggering paused update"; }
  };

  // entity, GUID
  class PreviewAnimation : public Event {
  public:
    PreviewAnimation(ECS::Entity entity, IGE::Assets::GUID guid) : Event(), mGUID{ guid }, mEntity{ entity } {}
    inline std::string GetName() const noexcept override { return "Previewing animation " + std::to_string(static_cast<uint64_t>(mGUID))
      + " for Entity " + mEntity.GetTag(); }

    IGE::Assets::GUID mGUID;
    ECS::Entity const mEntity;
  };
}

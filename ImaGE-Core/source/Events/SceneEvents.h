#pragma once
#include "Event.h"
#include <string>
#include <Scenes/SceneStates.h>

namespace Events
{

  // NewSceneState newState, std::string const& name
  class SceneStateChange : public Event
  {
  public:
    enum NewSceneState
    {
      NEW,
      CHANGED,
      STARTED,
      PAUSED,
      STOPPED
    };

    SceneStateChange(NewSceneState newState, std::string const& name)
      : Event(EventType::SCENE_STATE_CHANGE), mSceneName{ name }, mNewState { newState } {}
    inline std::string GetName() const noexcept override { return "Scene state of " + mSceneName + " changed to " + std::to_string(static_cast<int>(mNewState)); }

    std::string const mSceneName;
    NewSceneState const mNewState;
  };

  // name, path
  class LoadSceneEvent : public Event
  {
  public:
    LoadSceneEvent(std::string const& name, std::string const& path) : Event(EventType::LOAD_SCENE), mSceneName{ name }, mPath{ path } {}
    inline std::string GetName() const noexcept override { return "Loading Scene " + mSceneName; }

    std::string const mSceneName, mPath;
  };

  class UnloadSceneEvent : public Event
  {
  public:
    UnloadSceneEvent() : Event(EventType::UNLOAD_SCENE) {}
    inline std::string GetName() const noexcept override { return "Unloading Scene"; }
  };

  class LockMouseEvent : public Event
  {
  public:
    bool isLocked{ false };
    LockMouseEvent(bool lock) : Event(EventType::LOCK_MOUSE) { isLocked = lock; }
    inline std::string GetName() const noexcept override { return "Saving Scene"; }
  };


  class SaveSceneEvent : public Event
  {
  public:
    SaveSceneEvent(bool pretty) : Event(EventType::SAVE_SCENE), mPretty{ pretty } {}
    inline std::string GetName() const noexcept override { return "Saving Scene"; }

    bool const mPretty;
  };

  class SceneModifiedEvent : public Event
  {
  public:
    SceneModifiedEvent() : Event(EventType::SCENE_MODIFIED) {}
    inline std::string GetName() const noexcept override { return "Scene modified"; }
  };

  // name, path
  class EditPrefabEvent : public Event
  {
  public:
    EditPrefabEvent(std::string prefab, std::string path) : Event(EventType::EDIT_PREFAB), mPrefab{ std::move(prefab) }, mPath{ std::move(path) } {}
    inline std::string GetName() const noexcept override { return "Editing Prefab: " + mPrefab; }

    std::string const mPrefab, mPath;
  };

} // namespace Events

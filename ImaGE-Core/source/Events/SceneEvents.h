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
      : Event(), mSceneName{ name }, mNewState { newState } {}
    inline std::string GetName() const noexcept override { return "Scene state of " + mSceneName + " changed to " + std::to_string(static_cast<int>(mNewState)); }

    std::string const mSceneName;
    NewSceneState const mNewState;
  };

  // name, path
  class LoadSceneEvent : public Event
  {
  public:
    LoadSceneEvent(std::string const& name, std::string const& path) : Event(), mSceneName{ name }, mPath{ path } {}
    inline std::string GetName() const noexcept override { return "Loading Scene " + mSceneName; }

    std::string const mSceneName, mPath;
  };

  class UnloadSceneEvent : public Event
  {
  public:
    UnloadSceneEvent() : Event() {}
    inline std::string GetName() const noexcept override { return "Unloading Scene"; }
  };

  class LockMouseEvent : public Event
  {
  public:
    bool isLocked{ false };
    LockMouseEvent(bool lock) : Event() { isLocked = lock; }
    inline std::string GetName() const noexcept override { return "Saving Scene"; }
  };


  class SaveSceneEvent : public Event
  {
  public:
    SaveSceneEvent(bool pretty) : Event(), mPretty{ pretty } {}
    inline std::string GetName() const noexcept override { return "Saving Scene"; }

    bool const mPretty;
  };

  class SceneModifiedEvent : public Event
  {
  public:
    SceneModifiedEvent() : Event() {}
    inline std::string GetName() const noexcept override { return "Scene modified"; }
  };

  // name, path
  class EditPrefabEvent : public Event
  {
  public:
    EditPrefabEvent(std::string prefab, std::string path) : Event(), mPrefab{ std::move(prefab) }, mPath{ std::move(path) } {}
    inline std::string GetName() const noexcept override { return "Editing Prefab: " + mPrefab; }

    std::string const mPrefab, mPath;
  };

} // namespace Events

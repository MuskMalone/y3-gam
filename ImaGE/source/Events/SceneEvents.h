#pragma once
#include "Event.h"
#include <string>
#include <Scenes/SceneStates.h>

namespace Events
{

  class SceneStateChange : public Event
  {
  public:
    enum NewSceneState
    {
      NEW,
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

  class LoadSceneEvent : public Event
  {
  public:
    LoadSceneEvent(std::string const& name, std::string const& path) : Event(EventType::LOAD_SCENE), mSceneName{ name }, mPath{ path } {}
    inline std::string GetName() const noexcept override { return "Loading Scene " + mSceneName; }

    std::string const mSceneName, mPath;
  };

  class SaveSceneEvent : public Event
  {
  public:
    SaveSceneEvent() : Event(EventType::SAVE_SCENE) {}
    inline std::string GetName() const noexcept override { return "Saving Scene"; }
  };

  class EditPrefabEvent : public Event
  {
  public:
    EditPrefabEvent(std::string prefab, std::string path) : Event(EventType::EDIT_PREFAB), mPrefab{ std::move(prefab) }, mPath{ std::move(path) } {}
    inline std::string GetName() const noexcept override { return "Editing Prefab: " + mPrefab; }

    std::string const mPrefab, mPath;
  };

} // namespace Events

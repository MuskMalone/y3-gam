#pragma once
#include <Events/Event.h>
#include <GUI/Helpers/SceneEditorConfig.h>

namespace Events {
  class CollectEditorSceneData : public Event
  {
  public:
    CollectEditorSceneData() : Event(), mSceneConfig{}, mSceneName{} {}
    inline std::string GetName() const noexcept override { return "Saving Editor Configs for Scene " + mSceneName; }

    GUI::SceneEditorConfig mSceneConfig;
    std::string mSceneName;
  };

  // dispatched by GUIManager
  // GUI::SceneEditorConfig&&
  class LoadEditorSceneData : public Event
  {
  public:
    LoadEditorSceneData(GUI::SceneEditorConfig&& cfg) : Event(), mSceneConfig{ std::move(cfg) } {}
    inline std::string GetName() const noexcept override { return "Loading Editor Configs for Scene"; }

    GUI::SceneEditorConfig const mSceneConfig;
  };
}

#pragma once
#include <Events/Event.h>
#include <GUI/Helpers/SceneEditorConfig.h>
#include <filesystem>
#include <Asset/SmartPointer.h>

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

  // original, newFile
  class RenameAsset : public Event {
  public:
    RenameAsset(std::filesystem::path const& original, std::string newFile) : Event(), mOriginal{ original }, mNewFile{ std::move(newFile) } {}
    inline std::string GetName() const noexcept override { return "Renaming " + mOriginal.string() + " to " + mNewFile; }

    std::filesystem::path const mOriginal;
    std::string const mNewFile;
  };

  // guid
  class EditAnimation : public Event {
  public:
    EditAnimation(IGE::Assets::GUID guid) : Event(), mGUID{ guid } {}
    inline std::string GetName() const noexcept override { return "Editing animation " + std::to_string(static_cast<uint64_t>(mGUID)); }

    IGE::Assets::GUID mGUID;
  };

  class ToggleImGui : public Event {
  public:
    ToggleImGui() {}
    inline std::string GetName() const noexcept override { return "Toggling ImGui"; }
  };

  class QuitApplicationConfirmation : public Event {
  public:
    QuitApplicationConfirmation() : Event() {}
    inline std::string GetName() const noexcept override { return "Triggering quit application popup."; }
  };

  class QuitApplication : public Event {
  public:
    QuitApplication() : Event() {}
    inline std::string GetName() const noexcept override { return "Quitting application..."; }
  };
}

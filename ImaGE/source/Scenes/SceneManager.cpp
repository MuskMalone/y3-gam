#include "pch.h"
#include "SceneManager.h"
#include <filesystem>
#include <Events/EventManager.h>

#ifdef _DEBUG
#define EVENTS_DEBUG
#endif

namespace Scenes
{
  void SceneManager::Init()
  {
    // @TODO: SHOULD RETREIVE FROM CONFIG FILE IN FUTURE
    mTempDir = ".temp";

    // create temp directory if it doesn't already exist
    if (!std::filesystem::exists(mTempDir) || !std::filesystem::is_directory(mTempDir))
    {
      /*if (std::filesystem::create_directory(mTempDir)) {
        Debug::ErrorLogger::GetInstance().LogMessage("Created temp directory at: " + mTempDir);
      }
      else {
        Debug::ErrorLogger::GetInstance().LogError("Unable to create temp directory at: " + mTempDir + ". Scene reloading features may be unavailable!");
      }*/
    }

    // subscribe to scene events
    SUBSCRIBE_CLASS_FUNC(Events::EventType::NEW_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::START_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::PAUSE_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::STOP_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &SceneManager::HandleEvent, this);
  }

  //void SceneManager::Update()
  //{

  //}

  void SceneManager::LoadScene(std::string const& filePath)
  {

  }

  void SceneManager::InitScene()
  {

  }

  void SceneManager::ClearScene()
  {

  }

  void SceneManager::UnloadScene()
  {

  }

  EVENT_CALLBACK_DEF(SceneManager, HandleEvent)
  {
#ifdef EVENTS_DEBUG
    std::cout << "[SceneManager] Handled Event: " << event->GetName() << "\n";
#endif

    switch (event->GetCategory())
    {
    case Events::EventType::NEW_SCENE:
      ClearScene();
      UnloadScene();
      mSceneName = std::static_pointer_cast<Events::NewSceneEvent>(event)->mSceneName;
      break;
    case Events::EventType::START_SCENE:
      TemporarySave();
      mSceneState = SceneState::PLAYING;
      break;

    case Events::EventType::PAUSE_SCENE:
      mSceneState = SceneState::PAUSED;
      break;

    case Events::EventType::STOP_SCENE:
      LoadTemporarySave();
      InitScene();
      mSceneState = SceneState::STOPPED;
      break;

    case Events::EventType::EDIT_PREFAB:
      // save and unload
      TemporarySave();
      //UnloadScene();
      //FreeScene();
      break;

    default: break;
    }
  }

  void SceneManager::TemporarySave()
  {
    // we will differentiate save files using the size of the stack
    std::string const path{ mTempDir + std::to_string(mSaveStates.size()) };
    mSaveStates.emplace(mSceneName, path);
    //Serialization::Serializer::SerializeScene(path);
  }

  void SceneManager::LoadTemporarySave()
  {
    ClearScene();
    UnloadScene();

    auto saveState{ std::move(mSaveStates.top()) };
    mSaveStates.pop();
    mSceneName = std::move(saveState.mName);
    LoadScene(saveState.mPath);
    std::remove(saveState.mPath.c_str()); // delete the temp scene file
  }

} // namespace Scenes

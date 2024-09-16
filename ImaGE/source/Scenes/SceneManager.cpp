#include "pch.h"
#include "SceneManager.h"
#include <filesystem>
#include <Events/EventManager.h>
#include <Globals.h>
#include <Serialization/Serializer.h>
#include <Core/Entity.h>

#ifdef _DEBUG
#define EVENTS_DEBUG
#endif

namespace Scenes
{
  void SceneManager::Init()
  {
    mSceneState = SceneState::STOPPED;
    mObjFactory = &Reflection::ObjectFactory::GetInstance();
    // @TODO: SHOULD RETREIVE FROM CONFIG FILE IN FUTURE
    mTempDir = gTempDirectory;

    // create temp directory if it doesn't already exist
    if (!std::filesystem::exists(mTempDir) || !std::filesystem::is_directory(mTempDir))
    {
      if (std::filesystem::create_directory(mTempDir)) {
        // replace with logger
#ifdef _DEBUG
        std::cout << "Temp directory doesn't exist. Created at: " + mTempDir << "\n";
#endif
      }
      else {
        // replace with logger
#ifdef _DEBUG
        std::cout << "Unable to create temp directory at: " + mTempDir + ". Scene reloading features may be unavailable!" << "\n";
#endif
      }
    }

    // subscribe to scene events
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOAD_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SAVE_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &SceneManager::HandleEvent, this);
  }

  void SceneManager::PauseScene() {
    mSceneState = SceneState::PAUSED;
    QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::PAUSED, mSceneName);
  }

  void SceneManager::PlayScene() {
    TemporarySave();
    mSceneState = SceneState::PLAYING;
    QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::STARTED, mSceneName);
  }

  void SceneManager::StopScene() {
    // if no save states in stack, return to no scene
    // else it means we're loading back to a previous scene
    if (mSaveStates.empty()) {
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::STOPPED, mSceneName);
    }
    else {
      LoadTemporarySave();
      InitScene();
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::NEW, mSceneName);
    }

    mSceneState = SceneState::STOPPED;
  }

  void SceneManager::LoadScene(std::string const& path) {
    mObjFactory->LoadEntityData(path);
  }

  void SceneManager::InitScene() {
    mObjFactory->InitScene();
  }

  void SceneManager::ClearScene()
  {

  }

  void SceneManager::UnloadScene()
  {
    auto& entityMan { ECS::EntityManager::GetInstance() };
    std::vector<ECS::Entity> entitiesToDestroy;

    for (auto const& e : entityMan.GetAllEntities()) {
      if (entityMan.HasParent(e)) { continue; }

      entitiesToDestroy.emplace_back(e);
    }

    for (auto const& e : entitiesToDestroy) {
      entityMan.RemoveEntity(e);
    }
  }

  EVENT_CALLBACK_DEF(SceneManager, HandleEvent)
  {
#ifdef EVENTS_DEBUG
    std::cout << "[SceneManager] Handled Event: " << event->GetName() << "\n";
#endif

    switch (event->GetCategory())
    {
    case Events::EventType::LOAD_SCENE:
    {
      if (!mSceneName.empty()) {
        UnloadScene();
      }
      auto loadSceneEvent{ std::static_pointer_cast<Events::LoadSceneEvent>(event) };
      mSceneName = loadSceneEvent->mSceneName;
      if (!loadSceneEvent->mPath.empty()) {
        LoadScene(loadSceneEvent->mPath);
        InitScene();
      }

      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::NEW, mSceneName);
      break;
    }
    case Events::EventType::SAVE_SCENE:
      SaveScene();
      break;
    case Events::EventType::EDIT_PREFAB:
      // save and unload
      if (!mSceneName.empty()) {
        TemporarySave();
        ClearScene();
        UnloadScene();
      }
      mSceneState = SceneState::PREFAB_EDITOR;
      mSceneName = std::static_pointer_cast<Events::EditPrefabEvent>(event)->mPrefab;
      break;
    default: break;
    }
  }

  void SceneManager::SaveScene() const
  {
    // Save the scene
    std::ostringstream filepath{};
    filepath << gAssetsDirectory << "Scenes/" << mSceneName << sSceneFileExtension;
    Serialization::Serializer::SerializeScene(filepath.str());

    // replace with logger
#ifdef _DEBUG
   std::cout << "Successfully saved scene to " + filepath.str() << "\n";
#endif
  }

  void SceneManager::TemporarySave()
  {
    // we will differentiate save files using the size of the stack
    std::string const path{ mTempDir + "temp" + std::to_string(mSaveStates.size())};
    mSaveStates.emplace(mSceneName, path);
    Serialization::Serializer::SerializeScene(path);

    // replace with logger
#ifdef _DEBUG
    std::cout << "Temporarily saved scene to " + path << "\n";
#endif
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

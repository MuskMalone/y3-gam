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
      /*if (std::filesystem::create_directory(mTempDir)) {
        Debug::ErrorLogger::GetInstance().LogMessage("Created temp directory at: " + mTempDir);
      }
      else {
        Debug::ErrorLogger::GetInstance().LogError("Unable to create temp directory at: " + mTempDir + ". Scene reloading features may be unavailable!");
      }*/
    }

    // subscribe to scene events
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOAD_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::START_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::PAUSE_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::STOP_SCENE, &SceneManager::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &SceneManager::HandleEvent, this);
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
      break;
    }
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
      ClearScene();
      UnloadScene();
      mSceneState = SceneState::PREFAB_EDITOR;
      mSceneName = "Prefab Editor: " + std::static_pointer_cast<Events::EditPrefabEvent>(event)->mPrefab;
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

   // log("Successfully saved scene to " + filepath.str());
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

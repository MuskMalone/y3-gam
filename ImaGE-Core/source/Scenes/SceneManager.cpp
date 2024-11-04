/*!*********************************************************************
\file   SceneManager.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of class SceneManager, which is a singleton that
        manages the state of the scene in the engine.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "SceneManager.h"
#include <filesystem>
#include <Events/EventManager.h>
#include <Globals.h>
#include <Serialization/Serializer.h>
#include <Core/Entity.h>
#include <Physics/PhysicsSystem.h> //tch: this is to clear the physics rbs for now 
#include <Reflection/ObjectFactory.h>
#include "Graphics/RenderSystem.h"

#ifdef _DEBUG
//#define EVENTS_DEBUG
#endif

namespace Scenes
{
  SceneManager::SceneManager()
  {
    mSceneState = SceneState::STOPPED;
    // @TODO: SHOULD RETREIVE FROM CONFIG FILE IN FUTURE
    mTempDir = gTempDirectory;

    // create temp directory if it doesn't already exist
    if (!std::filesystem::exists(mTempDir) || !std::filesystem::is_directory(mTempDir))
    {
      if (std::filesystem::create_directory(mTempDir)) {
        Debug::DebugLogger::GetInstance().LogInfo("Temp directory doesn't exist. Created at: " + mTempDir);
      }
      else {
        Debug::DebugLogger::GetInstance().LogWarning("Unable to create temp directory at: " + mTempDir + ". Scene reloading features may be unavailable!");
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
      ClearScene();
      UnloadScene();
    }
    else {
      LoadTemporarySave();
      InitScene();
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::CHANGED, mSceneName);
    }

    mSceneState = SceneState::STOPPED;
  }

  void SceneManager::LoadScene(std::string const& path) {
    Reflection::ObjectFactory::GetInstance().LoadEntityData(path);
  }

  void SceneManager::InitScene() {
    Reflection::ObjectFactory::GetInstance().InitScene();
  }

  void SceneManager::ClearScene() {
    mSceneName.clear();
    IGE::Physics::PhysicsSystem::GetInstance()->ClearSystem();
  }

  void SceneManager::UnloadScene()
  {
    Reflection::ObjectFactory::GetInstance().ClearData();
    ECS::EntityManager::GetInstance().Reset();
  }

  EVENT_CALLBACK_DEF(SceneManager, HandleEvent)
  {
    switch (event->GetCategory())
    {
    case Events::EventType::LOAD_SCENE:
    {
      if (!mSceneName.empty()) {
        ClearScene();
        UnloadScene();
      }
      auto loadSceneEvent{ std::static_pointer_cast<Events::LoadSceneEvent>(event) };
      mSceneName = loadSceneEvent->mSceneName;
      if (!loadSceneEvent->mPath.empty()) {
        LoadScene(loadSceneEvent->mPath);
        InitScene();
        QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::CHANGED, mSceneName);
      }
      else {
        QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::NEW, mSceneName);
        Graphics::RenderSystem::mCameraManager.AddMainCamera();
      }
      Debug::DebugLogger::GetInstance().LogInfo("Loading scene: " + mSceneName + "...");
      
      break;
    }
    case Events::EventType::SAVE_SCENE:
      SaveScene();
      
      break;
    case Events::EventType::EDIT_PREFAB:
    {
      // save and unload
      if (!mSceneName.empty()) {
        TemporarySave();
        ClearScene();
        UnloadScene();
      }
      mSceneState = SceneState::PREFAB_EDITOR;
      auto prefabEvent{ std::static_pointer_cast<Events::EditPrefabEvent>(event) };
      mSceneName = prefabEvent->mPrefab;
      Debug::DebugLogger::GetInstance().LogInfo("Entering prefab editor for: " + prefabEvent->mPrefab + "...");
      break;
    }
    default: break;
    }
  }

  void SceneManager::SaveScene() const
  {
    // Save the scene
    std::ostringstream filepath{};
    filepath << gAssetsDirectory << "Scenes\\" << mSceneName << sSceneFileExtension;
    Serialization::Serializer::SerializeScene(filepath.str());

    Debug::DebugLogger::GetInstance().LogInfo("Successfully saved scene to " + filepath.str());
  }

  void SceneManager::TemporarySave()
  {
    // we will differentiate save files using the size of the stack
    std::string const path{ mTempDir + "temp" + std::to_string(mSaveStates.size())};
    mSaveStates.emplace(mSceneName, path);
    Serialization::Serializer::SerializeScene(path);

    //Debug::DebugLogger::GetInstance().LogInfo("Temporarily saved scene to " + path);
  }

  void SceneManager::LoadTemporarySave()
  {
    ClearScene();
    UnloadScene();

    auto saveState{ std::move(mSaveStates.top()) };
    mSaveStates.pop();
    mSceneName = std::move(saveState.mName);
    LoadScene(saveState.mPath);
    std::filesystem::remove(saveState.mPath); // delete the temp scene file

    Debug::DebugLogger::GetInstance().LogInfo("Scene reverted to previous state");
  }

  // cleanup any extra tmp files
  SceneManager::~SceneManager()
  {
    std::vector<std::filesystem::path> filesToRemove;
    for (auto const& file : std::filesystem::directory_iterator(mTempDir)) {
      filesToRemove.emplace_back(file.path());
    }

    for (auto const& file : filesToRemove) {
      std::filesystem::remove(file);
    }
  }

} // namespace Scenes

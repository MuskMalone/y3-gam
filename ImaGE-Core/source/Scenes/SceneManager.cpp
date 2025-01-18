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
#include <Core/Entity.h>
#include <Core/Components/Light.h>

#include <Events/EventManager.h>
#include <Reflection/ObjectFactory.h>
#include "Graphics/RenderSystem.h"
#include <Physics/PhysicsSystem.h>
#include <Serialization/Serializer.h>

#ifdef _DEBUG
//#define EVENTS_DEBUG
#endif

namespace Scenes
{
  SceneManager::SceneManager(SceneState startingState) : mTempDir{ gTempDirectory }, mSaveStates{},
    mScenes{}, mMainThreadQueue {}, mMainThreadQueueMutex{}, mSceneState{ startingState }
  {
    // @TODO: SHOULD RETREIVE FROM CONFIG FILE IN FUTURE
    //mTempDir = gTempDirectory;

#ifndef DISTRIBUTION
    // create temp directory if it doesn't already exist
    if (!std::filesystem::exists(mTempDir))
    {
      if (std::filesystem::create_directory(mTempDir)) {
        Debug::DebugLogger::GetInstance().LogInfo("Temp directory doesn't exist. Created at: " + mTempDir);
      }
      else {
        Debug::DebugLogger::GetInstance().LogWarning("Unable to create temp directory at: " + mTempDir + ". Scene reloading features may be unavailable!");
      }
    }
#endif

    // subscribe to scene events
    SUBSCRIBE_CLASS_FUNC(Events::LoadSceneEvent, &SceneManager::OnSceneLoad, this);
    SUBSCRIBE_CLASS_FUNC(Events::SaveSceneEvent, &SceneManager::OnSceneSave, this);
    SUBSCRIBE_CLASS_FUNC(Events::EditPrefabEvent, &SceneManager::OnPrefabEditor, this);
  }

  void SceneManager::PauseScene() {
    mSceneState = SceneState::PAUSED;
    QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::PAUSED, mScenes.front().mName);
  }

  void SceneManager::PlayScene() {
    TemporarySave(false);
    mSceneState = SceneState::PLAYING;
    QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::STARTED, mScenes.front().mName);
  }

  void SceneManager::StopScene() {
    Events::EventManager::GetInstance().DispatchImmediateEvent<Events::SceneStateChange>(Events::SceneStateChange::STOPPED, mScenes.front().mName);
    ClearScene();
    UnloadScene();

    // if no save states in stack, return to no scene
    // else it means we're loading back to a previous scene
    if (!mSaveStates.empty()) {
      LoadTemporarySave();
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::CHANGED, mScenes.front().mName);
    }

    mSceneState = SceneState::STOPPED;
  }

  void SceneManager::LoadScene(std::string const& path) {
    Reflection::ObjectFactory::GetInstance().LoadEntityData(path);
  }

  void SceneManager::InitScene(std::vector<ECS::Entity>& entityVector) {
    Reflection::ObjectFactory::GetInstance().InitScene(entityVector);
    // realign colliders with transforms
    IGE_EVENTMGR.DispatchImmediateEvent<Events::TriggerPausedUpdate>();
    Mono::ScriptManager::GetInstance().LinkAllScriptDataMember();
  }

  // i should just combine clear and unload functions
  void SceneManager::ClearScene() {
    mScenes.clear();
    IGE::Physics::PhysicsSystem::GetInstance()->ClearSystem();
    Graphics::MaterialTable::ClearMaterials();
  }

  void SceneManager::UnloadScene()
  {
    Reflection::ObjectFactory::GetInstance().ClearData();
    ECS::EntityManager::GetInstance().Reset();
  }

  void SceneManager::ReloadScene()
  {
    // trigger a temp save 
    TemporarySave(false);

    // load it back
    LoadTemporarySave();

    QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::NEW, mScenes.front().mName);
  }

  EVENT_CALLBACK_DEF(SceneManager, OnSceneLoad) {
    ClearScene();
    UnloadScene();

    auto loadSceneEvent{ std::static_pointer_cast<Events::LoadSceneEvent>(event) };
    Debug::DebugLogger::GetInstance().LogInfo("Loading scene: " + loadSceneEvent->mSceneName + "...");

    mScenes.emplace_back(loadSceneEvent->mSceneName);

    if (!loadSceneEvent->mPath.empty()) {
      LoadScene(loadSceneEvent->mPath);
      InitScene(mScenes.back().mEntities);
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::CHANGED, loadSceneEvent->mSceneName);
    }
    else {
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::NEW, loadSceneEvent->mSceneName);

      // add light to scene
      ECS::Entity newEntity{ IGE_ENTITYMGR.CreateEntity() };
      newEntity.SetTag("Directional Light");
      newEntity.EmplaceComponent<Component::Light>();
      newEntity.GetComponent<Component::Transform>().ApplyWorldRotation(-90.f, glm::vec3(1.f, 0.f, 0.f));  // face down by default
      // add camera
      Graphics::RenderSystem::mCameraManager.AddMainCamera();
    }

    // if the scene changed while playing, we dont stop it
    if (mSceneState != SceneState::PLAYING) {
      mSceneState = SceneState::STOPPED;
    }
    else {
      QUEUE_EVENT(Events::SceneStateChange, Events::SceneStateChange::CHANGED, loadSceneEvent->mSceneName);
    }
  }

  EVENT_CALLBACK_DEF(SceneManager, OnSceneSave) {
    SaveScene(CAST_TO_EVENT(Events::SaveSceneEvent)->mPretty);
  }

  EVENT_CALLBACK_DEF(SceneManager, OnPrefabEditor) {
    // save and unload
    if (!mScenes.empty()) {
      TemporarySave(false);
      ClearScene();
      UnloadScene();
    }
    mSceneState = SceneState::PREFAB_EDITOR;
    auto prefabEvent{ std::static_pointer_cast<Events::EditPrefabEvent>(event) };
    Debug::DebugLogger::GetInstance().LogInfo("Entering prefab editor for: " + prefabEvent->mPrefab + "...");
  }

  void SceneManager::BackupSave(bool pretty) const {
    // create backup directory if it doesn't already exist
    if (!std::filesystem::exists(gBackupDirectory))
    {
      if (std::filesystem::create_directory(gBackupDirectory)) {
        Debug::DebugLogger::GetInstance().LogInfo(std::string("Backup directory doesn't exist. Created at: ") + gBackupDirectory);
      }
      else {
        Debug::DebugLogger::GetInstance().LogWarning("Unable to create temp directory at: " + std::string(gBackupDirectory) + ". Scene reloading features may be unavailable!");
      }
    }

    for (Scene const& scene : mScenes) {
      std::string filePath{ gBackupDirectory + scene.mName + gSceneFileExt };
      Serialization::Serializer::SerializeScene(filePath, pretty ? Serialization::PRETTY : Serialization::COMPACT);
    }
  }

  void SceneManager::BackupCopy(std::string const& path) const {
    // No files should be copied/created in the distribution build
#ifndef DISTRIBUTION
    // create backup directory if it doesn't already exist
    if (!std::filesystem::exists(gBackupDirectory))
    {
      if (std::filesystem::create_directory(gBackupDirectory)) {
        Debug::DebugLogger::GetInstance().LogInfo(std::string("Backup directory doesn't exist. Created at: ") + gBackupDirectory);
      }
      else {
        Debug::DebugLogger::GetInstance().LogWarning("Unable to create temp directory at: " + std::string(gBackupDirectory) + ". Scene reloading features may be unavailable!");
      }
    }

    // now copy the file over
    if (std::filesystem::exists(path)) {
      std::filesystem::copy(path, gBackupDirectory, std::filesystem::copy_options::overwrite_existing);
    }
#endif
  }

  void SceneManager::SaveScene(bool pretty) const
  {
    // Save the scenes
    for (Scene const& scene : mScenes) {
      std::ostringstream filepath{};
      filepath << gAssetsDirectory << "Scenes\\" << scene.mName << gSceneFileExt;

      BackupCopy(filepath.str());
      Serialization::Serializer::SerializeScene(filepath.str(), pretty ? Serialization::PRETTY : Serialization::COMPACT);

      Debug::DebugLogger::GetInstance().LogInfo("Successfully saved scene to " + filepath.str());
    }
  }

  void SceneManager::TemporarySave(bool pretty)
  {
    SaveState saveState{};
    for (Scene const& scene : mScenes) {
      std::string path{ mTempDir + scene.mName + ".tmp" };
      Serialization::Serializer::SerializeScene(path, pretty ? Serialization::PRETTY : Serialization::COMPACT);
      saveState.saves.emplace_back(scene.mName, std::move(path));

      //Debug::DebugLogger::GetInstance().LogInfo("Temporarily saved scene to " + path);
    }

    mSaveStates.emplace(std::move(saveState));
  }

  void SceneManager::LoadTemporarySave()
  {
    ClearScene();
    UnloadScene();

    SaveState saveState{ std::move(mSaveStates.top()) };
    mSaveStates.pop();

    for (auto const& [name, path] : saveState.saves) {
      mScenes.emplace_back(name);

      LoadScene(path);
      InitScene(mScenes.back().mEntities);

      std::filesystem::remove(path); // delete the temp scene file
    }

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


  void SceneManager::SubmitToMainThread(const std::function<void()>& function)
  {
    std::scoped_lock<std::mutex> lock(mMainThreadQueueMutex);
    mMainThreadQueue.emplace_back(function);
  }

  void SceneManager::ExecuteMainThreadQueue()
  {
    std::scoped_lock<std::mutex> lock(mMainThreadQueueMutex);

    for (auto& func : mMainThreadQueue)
      func();

    mMainThreadQueue.clear();
  }

} // namespace Scenes

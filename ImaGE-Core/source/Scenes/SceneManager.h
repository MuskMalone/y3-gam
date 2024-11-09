/*!*********************************************************************
\file   SceneManager.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of class SceneManager, which is a singleton that
        manages the state of the scene in the engine.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Singleton/ThreadSafeSingleton.h>
#include "SceneStates.h"
#include <string>
#include <stack>
#include <Events/EventCallback.h>
#include <functional>

#define IGE_SCENEMGR Scenes::SceneManager::GetInstance()

namespace Scenes
{

  class SceneManager : public ThreadSafeSingleton<SceneManager>
  {
  public:

    /*!*********************************************************************
    \brief
      Initializes the SceneManager
    ************************************************************************/
    SceneManager();

    /*!*********************************************************************
    \brief
      Shutsdown the SceneManager and clears its contents
    ************************************************************************/
    ~SceneManager();

    /*!*********************************************************************
    \brief
      Stops the scene
    ************************************************************************/
    void StopScene();

    /*!*********************************************************************
    \brief
      Pauses the scene
    ************************************************************************/
    void PauseScene();

    /*!*********************************************************************
    \brief
      Plays the scene
    ************************************************************************/
    void PlayScene();

    /*!*********************************************************************
    \brief
      Reloads the scene
    ************************************************************************/
    void ReloadScene();

    /*!*********************************************************************
    \brief
      Gets the name of the current scene
    \return
      The name of the current scene
    ************************************************************************/
    inline std::string const& GetSceneName() const noexcept { return mSceneName; }

    /*!*********************************************************************
    \brief
      Gets the current state of the scene
      *SceneState makes use of bitflags for easy checking
    \return
      The state of the scene in the form of a SceneState enum
    ************************************************************************/
    inline SceneState GetSceneState() const noexcept { return mSceneState; }

    /*!*********************************************************************
    \brief
      Checks if the scene is playing or paused
    \return
      True if a scene is in play and false otherwise
    ************************************************************************/
    inline bool IsSceneInProgress() const noexcept { return mSceneState & (SceneState::PAUSED | SceneState::PLAYING); }

    /*!*********************************************************************
    \brief
      Check if there is currently a scene selected
    \return
      True if a sccene is selected and false otherwise
    ************************************************************************/
    inline bool NoSceneSelected() const noexcept { return mSceneState == SceneState::NO_SCENE; }


    void SubmitToMainThread(const std::function<void()>& function);
    void ExecuteMainThreadQueue();
    void BackupSave() const;

  private:
    struct SaveState
    {
      SaveState(std::string name, std::string path) : mName{ std::move(name) }, mPath{ std::move(path) } {}

      std::string mName, mPath;
    };

    static constexpr char sSceneFileExtension[] = ".scn";

    /*!*********************************************************************
    \brief
      Saves the current scene
    ************************************************************************/
    void SaveScene() const;

    /*!*********************************************************************
    \brief
      Loads a scene
    \param path
      The filepath of the scene file
    ************************************************************************/
    void LoadScene(std::string const& path);

    /*!*********************************************************************
    \brief
      Initializes a scene
    ************************************************************************/
    void InitScene();

    /*!*********************************************************************
    \brief
      Clears the scene
    ************************************************************************/
    void ClearScene();

    /*!*********************************************************************
    \brief
      Unloads the scene
    ************************************************************************/
    void UnloadScene();
    
    /*!*********************************************************************
    \brief
      Temporarily saves the scene to m_tempPath and stores the scene name
      in m_tempScene.
    ************************************************************************/
    void TemporarySave();

    /*!*********************************************************************
    \brief
      Loads the scene back from m_tempPath and restores the scene name to
      m_tempScene.
    ************************************************************************/
    void LoadTemporarySave();
  
    void BackupCopy(std::string const& path) const;

    /*!*********************************************************************
    \brief
      Handles the events the SceneManager subscribed to
      
      LOAD_SCENE
        - Sets the current scene name
      START_SCENE
        - Trigger a temporary save before the scene is played in the
          editor (to later revert to)
      STOP_SCENE
        - Revert to the temporary save when the scene is stopped in
          the editor
      EDIT_PREFAB
        - Temporarily saves the scene and pushes it to the stack before
          transitioning to prefab editor mode

    \param event
      The event to handle
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);

    std::stack<SaveState> mSaveStates;  // used to temporarily store scene saves when playing/stopping/transitioning to PrefabEditor
    std::vector<std::function<void()>> mMainThreadQueue;
    std::string mSceneName, mTempDir;
    std::mutex mMainThreadQueueMutex;
    SceneState mSceneState;
  };

};  // namespace Scenes

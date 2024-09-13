#pragma once
#include <Singleton.h>
#include "SceneStates.h"
#include <string>
#include <stack>
#include <Events/EventCallback.h>

namespace Scenes
{

  class SceneManager : public Singleton<SceneManager>
  {
  public:

    void Init();
    //void Update();
    inline std::string const& GetSceneName() const noexcept { return mSceneName; }
    inline SceneState GetSceneState() const noexcept { return mSceneState; }

  private:
    struct SaveState
    {
      SaveState(std::string name, std::string path) : mName{ std::move(name) }, mPath{ std::move(path) } {}

      std::string const mName, mPath;
    };

    void LoadScene(std::string const& filePath);
    void InitScene();
    void ClearScene();
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

    /*!*********************************************************************
    \brief
      Handles the events the SceneManager subscribed to
      
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
    std::string mSceneName, mTempDir;
    SceneState mSceneState;
  };

};  // namespace Scenes

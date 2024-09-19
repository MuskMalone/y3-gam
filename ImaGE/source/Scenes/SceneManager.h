#pragma once
#include <Singleton.h>
#include "SceneStates.h"
#include <string>
#include <stack>
#include <Events/EventCallback.h>
#include <Reflection/ObjectFactory.h>

namespace Scenes
{

  class SceneManager : public Singleton<SceneManager>
  {
  public:

    void Init();

    void StopScene();
    void PauseScene();
    void PlayScene();

    inline std::string const& GetSceneName() const noexcept { return mSceneName; }
    // SceneState makes use of bitflags for easy checking
    inline SceneState GetSceneState() const noexcept { return mSceneState; }
    inline bool IsScenePlaying() const noexcept { return mSceneState & (Scenes::SceneState::PAUSED | Scenes::SceneState::PLAYING); }
    // check if there is currently a scene selected
    inline bool NoSceneSelected() const noexcept { return mSceneName.empty(); }

  private:
    struct SaveState
    {
      SaveState(std::string name, std::string path) : mName{ std::move(name) }, mPath{ std::move(path) } {}

      std::string mName, mPath;
    };

    static constexpr char sSceneFileExtension[] = ".scn";

    void SaveScene() const;
    void LoadScene(std::string const& path);
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
    std::string mSceneName, mTempDir;
    Reflection::ObjectFactory* mObjFactory;
    SceneState mSceneState{};
  };

};  // namespace Scenes

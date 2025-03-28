/*!*********************************************************************
\file   Toolbar.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the main menubar of the
        editor. Contains options related to files and editor settings.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <GUI/GUIWindow.h>
#include <vector>
#include <memory>
#include <Events/EventCallback.h>
#include <EditorCamera.h>

namespace GUI {
  class Toolbar : public GUIWindow {
  public:
    Toolbar(const char* name,
      std::vector<std::shared_ptr<GUIWindow>> const& windowsRef,
      std::weak_ptr<Graphics::EditorCamera> editorCam);

    /*!*********************************************************************
    \brief
      Runs the toolbar
    ************************************************************************/
    void Run() override;

  private:
    std::vector<std::shared_ptr<GUIWindow>> const& mWindowsRef;  // to manage active states
    std::weak_ptr<Graphics::EditorCamera> mEditorCam;
    bool mScenePopup, mPrefabPopup;
    bool mDisableAll, mAllowCreationOnly;

    /*!*********************************************************************
    \brief
      Runs the popup menu for a new scene
    ************************************************************************/
    void RunNewScenePopup();

    /*!*********************************************************************
    \brief
      Runs the popup menu for a new prefab
    ************************************************************************/
    void RunNewPrefabPopup();

    /*!*********************************************************************
    \brief
      Handles the events the Toolbar subscribed to.

      SCENE_STATE_CHANGE
        - Disables certain menu options based on the current scene state
    \param event
      The event to handle
    ************************************************************************/
    EVENT_CALLBACK_DECL(OnSceneStateChange);
    EVENT_CALLBACK_DECL(OnPrefabEdit);

    void HelpMenu(const char* label) const;
    void SerializationMenu(const char* label) const;
    void ResavePrefabsMenuItem(const char* label) const;
    void ReimportMeshesMenuItem(const char* label) const;
    bool LightingMenu(const char* label, bool disabled) const;
    bool EditorCamMenu(const char* label, bool disabled) const;
  };
} // namespace GUI

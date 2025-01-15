/*!*********************************************************************
\file   GUIManager.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of class GUIManager, which holds the instance to
        all GUI classes running in the editor. It is responsible for
        updating them every game loop and is the main class that
        the application interfaces with. Also holds shared variables
        for window elements to access.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>
#include <GUI/GUIWindow.h>
#include <Graphics/RenderTarget.h>
#include "GUI/Dockable/GameViewport.h"
#include <Events/EventCallback.h>

namespace GUI {
  class Viewport;
  class GameViewport;

  class GUIManager {
  public:
    GUIManager();
    
    /*!*********************************************************************
    \brief
      Initializes the GUIManager along with all other GUI elements
    \param renderTarget
      Reference to the render target holding the framebuffer and editor
      camera to initialize the viewport class
    ************************************************************************/
    void Init(Graphics::RenderTarget& renderTarget);

    /*!*********************************************************************
    \brief
      Updates all GUI elements managed by the class
    \param framebuffer
      Theframebuffer to be passed into the viewport
    ************************************************************************/
    void UpdateGUI(std::shared_ptr<Graphics::Framebuffer> const& framebuffer, std::shared_ptr<Graphics::Texture> const& tex);
    
    /*!*********************************************************************
    \brief
      Shutsdown the GUIManager and releases all GUI windows
    ************************************************************************/
    void Shutdown();
    
    inline bool IsGameViewActive() const { return mGameViewport->IsActive(); }

  private:
    EVENT_CALLBACK_DECL(OnSceneSave);
    EVENT_CALLBACK_DECL(OnSceneLoad);
    EVENT_CALLBACK_DECL(OnCollectEditorData);

    std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    std::vector<std::shared_ptr<GUIWindow>> mWindows; // dockable/hideable windows
    std::shared_ptr<Viewport> mEditorViewport;  // ptr to the viewport in mWindows
    std::shared_ptr<GameViewport> mGameViewport; // ptr to gameviewport
  };

} // namespace GUI

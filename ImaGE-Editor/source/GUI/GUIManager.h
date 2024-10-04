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
#ifndef IMGUI_DISABLE
#include <vector>
#include <memory>
#include <GUI/GUIWindow.h>
#include "Styles/Styler.h"
#include <Graphics/RenderTarget.h>
#include <Core/Entity.h>

namespace GUI {
  class Viewport;

  class GUIManager {
  public:

    GUIManager();
    
    /*!*********************************************************************
    \brief
      Initializes the GUIManager along with all other GUI elements
    ************************************************************************/
    void Init();

    /*!*********************************************************************
    \brief
      Updates all GUI elements managed by the class
    \param renderTarget
      The render target to be passed into the viewport
    ************************************************************************/
    void UpdateGUI(Graphics::RenderTarget& renderTarget);
    
    /*!*********************************************************************
    \brief
      Shutsdown the GUIManager and releases all GUI windows
    ************************************************************************/
    void Shutdown();

    /*!*********************************************************************
    \brief
      Gets the styler of the engine
    \return
      Reference to the Styler
    ************************************************************************/
    static inline Styler& GetStyler() noexcept { return mStyler; }

    /*!*********************************************************************
    \brief
      Gets the currently selected entity in the editor
    \return
      The currently selected entity
    ************************************************************************/
    static inline ECS::Entity GetSelectedEntity() noexcept { return sSelectedEntity; }

    /*!*********************************************************************
    \brief
      Sets the currently selected entity in the editor
    \param entity
      The entity to set
    ************************************************************************/
    static inline void SetSelectedEntity(ECS::Entity const& entity) noexcept { sSelectedEntity = entity; }
    
  private:
    std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    std::vector<std::shared_ptr<GUIWindow>> mWindows; // dockable/hideable windows
    std::shared_ptr<Viewport> mEditorViewport;  // ptr to the viewport in mWindows

    static Styler mStyler; // handles editor's styles
    static ECS::Entity sSelectedEntity; // currently selected entity
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

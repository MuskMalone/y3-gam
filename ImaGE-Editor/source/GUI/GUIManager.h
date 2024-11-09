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
#include "GUI/Dockable/GameViewport.h"
#include <unordered_set>

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


    static inline std::unordered_set<ECS::Entity::EntityID> const& GetSelectedEntities() noexcept { return sSelectedEntities; }
    static inline bool IsEntitySelected(ECS::Entity const& entity) { return sSelectedEntities.contains(entity.GetRawEnttEntityID()); }
    static inline void AddSelectedEntity(ECS::Entity const& entity) { sSelectedEntities.emplace(entity.GetRawEnttEntityID()); }
    static void RemoveSelectedEntity(ECS::Entity const& entity) { sSelectedEntities.erase(entity.GetRawEnttEntityID()); }
    static void ClearSelectedEntities() { sSelectedEntities.clear(); }
    
    inline bool IsGameViewActive() {
        return mGameViewport->IsActive();
    }
  private:
    std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    std::vector<std::shared_ptr<GUIWindow>> mWindows; // dockable/hideable windows
    std::shared_ptr<Viewport> mEditorViewport;  // ptr to the viewport in mWindows
    std::shared_ptr<GameViewport> mGameViewport; // ptr to gameviewport

    inline static Styler mStyler; // handles editor's styles
    inline static std::unordered_set<ECS::Entity::EntityID> sSelectedEntities;
    inline static ECS::Entity sSelectedEntity; // currently selected entity
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

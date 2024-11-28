/*!*********************************************************************
\file   GUIManager.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of class GUIManager, which holds the instance to
        all GUI classes running in the editor. It is responsible for
        updating them every game loop and is the main class that
        the application interfaces with. Also holds shared variables
        for window elements to access.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "GUIManager.h"
#include <ImGui/imgui.h>
#include <Core/Entity.h>
#include "Graphics/Texture.h"
#include <GUI/GUIVault.h>

#pragma region IndivWindowIncludes
#include "Dockable/AssetBrowser.h"
#include "Dockable/Console.h"
#include "Dockable/Inspector.h"
#include "Dockable/Debugging/PerformanceWindow.h"
#include "Dockable/Debugging/RenderPassViewer.h"
#include "Persistent/PrefabEditor.h"
#include "Persistent/SceneControls.h"
#include "Dockable/SceneHierarchy.h"
#include "Persistent/Toolbar.h"
#include "Dockable/Viewport.h"
#include "Dockable/Layers.h"
#include "Dockable/GameViewport.h"
#include "Dockable/PostProcessingSettings.h"
#pragma endregion

namespace GUI {
  GUIManager::GUIManager() : mPersistentElements{}, mWindows{}, mEditorViewport{} {
  
  }

  void GUIManager::Init(Graphics::RenderTarget& renderTarget) {
    mPersistentElements.reserve(3);
    mPersistentElements.emplace_back(std::make_unique<Toolbar>("Toolbar", mWindows));
    mPersistentElements.emplace_back(std::make_unique<SceneControls>("Scene Controls"));
    mPersistentElements.emplace_back(std::make_unique<PrefabEditor>("Prefab Editor"));

    // hold ptrs to the viewports
    mEditorViewport = std::make_shared<Viewport>("Viewport", renderTarget.camera);
    mGameViewport = std::make_shared<GameViewport>("Game View");

    mWindows.reserve(10);
    mWindows.emplace_back(mEditorViewport); // viewports should always be first
    mWindows.emplace_back(mGameViewport);

    mWindows.emplace_back(std::make_shared<Inspector>("Inspector"));
    mWindows.emplace_back(std::make_shared<SceneHierarchy>("Scene Hierarchy"));
    mWindows.emplace_back(std::make_shared<AssetBrowser>("Asset Browser"));
    mWindows.emplace_back(std::make_shared<Console>("Console"));
    mWindows.emplace_back(std::make_shared<PerformanceWindow>("Performance Window"));
    mWindows.emplace_back(std::make_shared<LayerWindow>("Layers"));
    mWindows.emplace_back(std::make_shared<RenderPassViewer>("Render Pass Viewer"))->Toggle();  // default to non-active
    mWindows.emplace_back(std::make_shared<PostProcessingSettings>("Post Processing"));

    Styler& styler{ GUIVault::GetStyler() };
    styler.LoadFonts();
    styler.SetCurrentTheme(static_cast<CustomTheme>(gEditorDefaultTheme)); // Default theme should be read from settings file
  }

  void GUIManager::UpdateGUI(std::shared_ptr<Graphics::Framebuffer> const& framebuffer, std::shared_ptr<Graphics::Texture> const& tex) {
    // TOP SECRET MENU - Developer Tools
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_LeftShift)
      && ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) {
      GUIVault::sDevTools = !GUIVault::sDevTools;
    }

    // Always run persistent windows
    for (auto const& elem : mPersistentElements) {
      elem->Run();
    }

    // Run all active windows except viewport
    for (unsigned i{ 2 }; i < mWindows.size(); ++i) {
      if (!mWindows[i]->IsActive()) { continue; }
      mWindows[i]->Run();
    }

    if (mGameViewport->IsActive()) {
      mGameViewport->Render(tex);
    }

    // Update viewport if active
    if (mEditorViewport->IsActive()) {
      mEditorViewport->Render(framebuffer);
    }
  }

  void GUIManager::Shutdown() {
    GUIVault::GetStyler().Shutdown();

    mEditorViewport.reset();
    mPersistentElements.clear();
    mWindows.clear();
  }




} // namespace GUI

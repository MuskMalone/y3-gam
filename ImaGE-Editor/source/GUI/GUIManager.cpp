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
#pragma region IndivWindowIncludes
#include "Dockable/AssetBrowser.h"
#include "Dockable/Console.h"
#include "Dockable/Inspector.h"
#include "Dockable/PerformanceWindow.h"
#include "Persistent/PrefabEditor.h"
#include "Persistent/SceneControls.h"
#include "Dockable/SceneHierarchy.h"
#include "Persistent/Toolbar.h"
#include "Dockable/Viewport.h"
#pragma endregion

namespace GUI {
  // Static Initialization
  ECS::Entity GUIManager::sSelectedEntity{};
  Styler GUIManager::mStyler{};

  GUIManager::GUIManager() : mPersistentElements{}, mWindows{}, mEditorViewport{} {
  
  }

  void GUIManager::Init() {
    mPersistentElements.reserve(3);
    mPersistentElements.emplace_back(std::make_unique<Toolbar>("Toolbar", mWindows));
    mPersistentElements.emplace_back(std::make_unique<SceneControls>("Scene Controls"));
    mPersistentElements.emplace_back(std::make_unique<PrefabEditor>("Prefab Editor"));

    auto vp{ std::make_shared<Viewport>("Viewport") };
    mEditorViewport = vp; // hold a ptr to the viewport

    mWindows.reserve(5);
    mWindows.emplace_back(std::move(vp)); // viewport should always be first

    mWindows.emplace_back(std::make_shared<Inspector>("Inspector"));
    mWindows.emplace_back(std::make_shared<SceneHierarchy>("Scene Hierarchy"));
    mWindows.emplace_back(std::make_shared<AssetBrowser>("Asset Browser"));
    mWindows.emplace_back(std::make_shared<Console>("Console"));
    mWindows.emplace_back(std::make_shared<PerformanceWindow>("Performance Window"));

    mStyler.LoadFonts();
    mStyler.SetCurrentTheme(static_cast<CustomTheme>(gEditorDefaultTheme)); // Default theme should be read from settings file
  }

  void GUIManager::UpdateGUI(Graphics::RenderTarget& renderTarget) {
    // Always run persistent windows
    for (auto const& elem : mPersistentElements) {
      elem->Run();
    }

    // Run all active windows except viewport
    for (unsigned i{ 1 }; i < mWindows.size(); ++i) {
      if (!mWindows[i]->IsActive()) { continue; }
      mWindows[i]->Run();
    }

    // Update viewport if active
    if (mEditorViewport->IsActive()) {
      mEditorViewport->Render(renderTarget);
    }
  }

  void GUIManager::Shutdown() {
    mStyler.Shutdown();
    sSelectedEntity = {};

    mEditorViewport.reset();
    mPersistentElements.clear();
    mWindows.clear();
  }

} // namespace GUI

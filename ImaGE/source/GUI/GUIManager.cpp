#include <pch.h>
#ifndef IMGUI_DISABLE
#include "GUIManager.h"
#include <ImGui/imgui.h>
#include <Core/Entity.h>
#pragma region IndivWindowIncludes
#include "Persistent/Toolbar.h"
#include "Persistent/SceneControls.h"
#include "Persistent/PrefabEditor.h"
#include "Dockable/Viewport.h"
#include "Dockable/Inspector.h"
#include "Dockable/SceneHierarchy.h"
#include "Dockable/AssetBrowser.h"
#include "Dockable/Console.h"
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

    mWindows.reserve(5);
    auto vp{ std::make_shared<Viewport>("Viewport") };
    mEditorViewport = vp; // hold a ptr to the viewport
    mWindows.emplace_back(std::move(vp)); // viewport should always be first

    mWindows.emplace_back(std::make_shared<Inspector>("Inspector"));
    mWindows.emplace_back(std::make_shared<SceneHierarchy>("Scene Hierarchy"));
    mWindows.emplace_back(std::make_shared<AssetBrowser>("Asset Browser"));
    mWindows.emplace_back(std::make_shared<Console>("Console"));

    mStyler.LoadFonts();
    mStyler.SetCurrentTheme(static_cast<CustomTheme>(gEditorDefaultTheme)); // Default theme should be read from settings file
  }

  void GUIManager::UpdateGUI(std::shared_ptr<Graphics::Framebuffer> const& framebuffer) {
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
      mEditorViewport->Update(framebuffer);
    }
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

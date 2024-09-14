#include <pch.h>
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
#pragma endregion

namespace GUI
{
  ECS::Entity GUIManager::mSelectedEntity{};
  std::vector<std::unique_ptr<GUIWindow>> GUIManager::mPersistentElements, GUIManager::mWindows;

  void GUIManager::Init(Graphics::Framebuffer const& framebuffer)
  {
    mPersistentElements.reserve(3);
    mPersistentElements.emplace_back(std::make_unique<Toolbar>("Toolbar", mWindows));
    mPersistentElements.emplace_back(std::make_unique<SceneControls>("Scene Controls"));
    mPersistentElements.emplace_back(std::make_unique<PrefabEditor>("Prefab Editor"));

    mWindows.reserve(4);
    mWindows.emplace_back(std::make_unique<Viewport>("Viewport", framebuffer));
    mWindows.emplace_back(std::make_unique<Inspector>("Inspector"));
    mWindows.emplace_back(std::make_unique<SceneHierarchy>("Scene Hierarchy"));
    mWindows.emplace_back(std::make_unique<AssetBrowser>("Asset Browser"));
  }

  void GUIManager::UpdateGUI()
  {
    // always run persistent windows
    for (auto const& elem : mPersistentElements)
    {
      elem->Run();
    }

    // run all active windows
    for (auto const& window : mWindows)
    {
      if (!window->IsActive()) { continue; }

      window->Run();
    }
  }

} // namespace GUI

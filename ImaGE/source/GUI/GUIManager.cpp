#include <pch.h>
#include "GUIManager.h"
#include <ImGui/imgui.h>
#pragma region IndivWindowIncludes
#include "Persistent/Toolbar.h"
#include "Persistent/SceneControls.h"
#include "Dockable/Viewport.h"
#include "Dockable/Inspector.h"
#include "Dockable/SceneHierarchy.h"
#include "Dockable/AssetBrowser.h"
#pragma endregion

namespace GUI
{
  std::vector<std::unique_ptr<GUIWindow>> GUIManager::m_persistentElements, GUIManager::m_windows;

  void GUIManager::Init(Graphics::Framebuffer const& framebuffer)
  {
    m_persistentElements.reserve(2);
    m_persistentElements.emplace_back(std::make_unique<Toolbar>("Scene Controls", m_windows));
    m_persistentElements.emplace_back(std::make_unique<SceneControls>("Scene Controls"));

    m_windows.reserve(4);
    m_windows.emplace_back(std::make_unique<Viewport>("Viewport", framebuffer));
    m_windows.emplace_back(std::make_unique<Inspector>("Inspector"));
    m_windows.emplace_back(std::make_unique<SceneHierarchy>("Scene Hierarchy"));
    m_windows.emplace_back(std::make_unique<AssetBrowser>("Asset Browser"));
  }

  void GUIManager::UpdateGUI()
  {
    // always run persistent windows
    for (auto& elem : m_persistentElements)
    {
      elem->Run();
    }

    // run all active windows
    for (auto& window : m_windows)
    {
      if (!window->IsActive()) { continue; }

      window->Run();
    }
  }

} // namespace GUI

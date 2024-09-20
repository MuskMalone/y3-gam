#include <pch.h>
#ifndef IMGUI_DISABLE
#include "GUIManager.h"
#include <ImGui/imgui.h>
#include <Core/Entity.h>
#include "Styles/FontAwesome6Icons.h"
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
  ECS::Entity GUIManager::sSelectedEntity{};

  GUIManager::GUIManager() :mPersistentElements{}, mWindows{} {}

  void GUIManager::Init(Graphics::Framebuffer const& framebuffer)
  {
    mPersistentElements.reserve(3);
    mPersistentElements.emplace_back(std::make_unique<Toolbar>("Toolbar", mWindows));
    mPersistentElements.emplace_back(std::make_unique<SceneControls>("Scene Controls"));
    mPersistentElements.emplace_back(std::make_unique<PrefabEditor>("Prefab Editor"));

    mWindows.reserve(5);
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

  void GUIManager::StyleGUI() const
  {
    ImGuiIO& io{ ImGui::GetIO() };
    io.Fonts->AddFontDefault();

    ImFontConfig cfg;
    cfg.MergeMode = true;
    cfg.GlyphMinAdvanceX = 13.f;  // make icons monospaced
    cfg.GlyphOffset = ImVec2(0, 1);
    static constexpr ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromFileTTF(gIconsFontPath, 17.f * 2.0f / 3.0f, &cfg, icon_ranges);
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

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
  // Static Initialization
  ECS::Entity GUIManager::sSelectedEntity{};
  std::vector<ImFont*> GUIManager::sCustomFonts{};

  GUIManager::GUIManager() :mPersistentElements{}, mWindows{} {}

  void GUIManager::Init(Graphics::Framebuffer const& framebuffer) {
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

  void GUIManager::UpdateGUI() {
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

  void GUIManager::StyleGUI() const {
    ImGuiIO& io = ImGui::GetIO();

    static constexpr float defaultFontSize{ 13.f };
    static constexpr float largeFontSize{ 24.f };
    static constexpr float smallIconFontSize{ 13.0f };
    static constexpr float largeIconFontSize{ 24.0f };

    ImFontConfig fontConfig;
    fontConfig.SizePixels = defaultFontSize;
    ImFont* defaultFont = io.Fonts->AddFontDefault(&fontConfig);
    GUIManager::FontAwesomeMerge(io, defaultFontSize);
    sCustomFonts.emplace_back(defaultFont);

    sCustomFonts.emplace_back(io.Fonts->AddFontFromFileTTF(gRobotoBoldFontPath, largeFontSize));
    GUIManager::FontAwesomeMerge(io, largeFontSize);
    sCustomFonts.emplace_back(io.Fonts->AddFontFromFileTTF(gRobotoMediumFontPath, defaultFontSize));
    GUIManager::FontAwesomeMerge(io, defaultFontSize);
    sCustomFonts.emplace_back(io.Fonts->AddFontFromFileTTF(gRobotoThinFontPath, defaultFontSize));
    GUIManager::FontAwesomeMerge(io, defaultFontSize);

    io.Fonts->Build();
  }

  void GUIManager::FontAwesomeMerge(ImGuiIO& io, float size) const {
    ImFontConfig iconFontConfig;
    iconFontConfig.MergeMode = true;
    iconFontConfig.GlyphMinAdvanceX = size;
    iconFontConfig.GlyphOffset = ImVec2(0, 1);

    static constexpr ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    io.Fonts->AddFontFromFileTTF(gIconsFontPath, size, &iconFontConfig, icon_ranges);
  }

} // namespace GUI

#endif  // IMGUI_DISABLE

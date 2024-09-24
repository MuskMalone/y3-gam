#pragma once
#ifndef IMGUI_DISABLE
#include <vector>
#include <memory>
#include <GUI/GUIWindow.h>
#include <Graphics/Framebuffer.h>
#include <Core/Entity.h>

namespace GUI {
  class GUIManager {
  public:

    GUIManager();

    // taking in framebuffer to pass into viewport class
    void Init(Graphics::Framebuffer const& framebuffer);
    void UpdateGUI();
    void StyleGUI() const;

    enum CustomFonts {
      Default,
      RobotoBold,
      RobotoMedium,
      RobotoThin,
      MontserratSemiBold,
      MontserratLight,
      MontserratRegular
    };

    enum class CustomTheme {
      Black,
      Cloudy,
      Coconut,
      NumItems
    };

    static inline std::string GetCustomThemeString(CustomTheme customTheme) { return sCustomThemeStringMap.at(customTheme); }
    static inline std::vector<ImFont*> const& GetCustomFonts() noexcept { return sCustomFonts; }
    static inline ECS::Entity const& GetSelectedEntity() noexcept { return sSelectedEntity; }
    static inline CustomTheme GetCurrentTheme() noexcept { return sCurrentTheme; }

    static inline void SetSelectedEntity(ECS::Entity const& entity) noexcept { sSelectedEntity = entity; }
    static inline void SetCurrentTheme(CustomTheme theme) noexcept { sCurrentTheme = theme; }
    
  private:
    void FontAwesomeMerge(ImGuiIO& io, float size) const;
    void AddCustomFontGUI(std::string const& fontPath, float size) const;

    // Themes
    void ApplyDarkTheme();
    void ApplyGrayTheme();
    void ApplyLightTheme();

  private:
    std::vector<std::unique_ptr<GUIWindow>> mPersistentElements;  // contains controls outside of the dockspace
    std::vector<std::unique_ptr<GUIWindow>> mWindows; // dockable/hideable windows

    static ECS::Entity sSelectedEntity; // currently selected entity
    static std::vector<ImFont*> sCustomFonts;
    static CustomTheme sCurrentTheme;
    static std::unordered_map<CustomTheme, std::string> sCustomThemeStringMap;
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

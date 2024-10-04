#pragma once
#ifndef IMGUI_DISABLE
#include "StyleEnums.h"

// forward declaration
struct ImFont;

namespace GUI
{

  class Styler
  {
  public:
    inline std::string const& GetCustomThemeString(CustomTheme customTheme) const { return mCustomThemes.at(customTheme).first; }
    inline ImFont* GetCustomFont(CustomFonts font) const noexcept { return mCustomFonts[font]; }
    inline CustomTheme GetCurrentTheme() const noexcept { return mCurrentTheme; }
    void SetCurrentTheme(CustomTheme theme);

  private:
    friend class GUIManager;  // allow GUIManager full access
    using ThemeFunction = void (Styler::*)(); // maybe should use std::function to simplify invocation in SetCurrentTheme

    Styler(); // only constructable by GUIManager
    void Shutdown();

    // CustomTheme, <ThemeName, ThemeFunction>
    std::map<CustomTheme, std::pair<std::string, ThemeFunction>> mCustomThemes; // using map so its ordered by enum
    std::vector<ImFont*> mCustomFonts;
    CustomTheme mCurrentTheme;

    inline static constexpr float sDefaultFontSize   = 18.f;
    inline static constexpr float sLargeFontSize     = 24.f;
    inline static constexpr float sSmallIconFontSize = 13.0f;
    inline static constexpr float sLargeIconFontSize = 24.0f;

    void LoadFonts();
    void AddCustomFontGUI(std::string const& fontPath, float size);

    // Themes
    void ApplyDarkTheme();
    void ApplyGrayTheme();
    void ApplyLightTheme();
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

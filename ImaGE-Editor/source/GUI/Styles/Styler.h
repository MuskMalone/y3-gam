/*!*********************************************************************
\file   Styler.h
\author 
\date   5-October-2024
\brief  Definition of class Styler, which encapsulates functions
        related to the styling of the editor.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
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
    friend class GUIVault; friend class GUIManager;  // allow GUIVault and GUIManager full access
    using ThemeFunction = void (Styler::*)(); // maybe should use std::function to simplify invocation in SetCurrentTheme

    Styler(); // only constructable by GUIVault

    /*!*********************************************************************
    \brief
      Shutsdown the Styler and clears its contents
    ************************************************************************/
    void Shutdown();

    /*!*********************************************************************
    \brief
      Loads the extra fonts to be used by the editor
    ************************************************************************/
    void LoadFonts();

    /*!*********************************************************************
    \brief
      Helper function called when loading fonts to merge a font sheet
    \param fontPath
      The path of the font
    \param size
      The size of the font
    ************************************************************************/
    void AddCustomFontGUI(std::string const& fontPath, float size);

    // Themes
    /*!*********************************************************************
    \brief
      Modifies ImGui styles to the dark theme
    ************************************************************************/
    void ApplyDarkTheme();

    /*!*********************************************************************
    \brief
      Modifies ImGui styles to the gray theme
    ************************************************************************/
    void ApplyGrayTheme();

    /*!*********************************************************************
    \brief
      Modifies ImGui styles to the light theme
    ************************************************************************/
    void ApplyLightTheme();

    // CustomTheme, <ThemeName, ThemeFunction>
    std::map<CustomTheme, std::pair<std::string, ThemeFunction>> mCustomThemes; // using map so its ordered by enum
    std::vector<ImFont*> mCustomFonts;
    CustomTheme mCurrentTheme;

    inline static constexpr float sDefaultFontSize = 18.f;
    inline static constexpr float sLargeFontSize = 24.f;
    inline static constexpr float sSmallIconFontSize = 13.0f;
    inline static constexpr float sLargeIconFontSize = 24.0f;
  };

} // namespace GUI

#endif  // IMGUI_DISABLE

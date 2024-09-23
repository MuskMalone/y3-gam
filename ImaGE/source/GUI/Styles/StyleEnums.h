#pragma once
#ifndef IMGUI_DISABLE

namespace GUI {
  enum CustomFonts {
    DEFAULT = 0,
    ROBOTO_BOLD,
    ROBOTO_MEDIUM,
    ROBOTO_THIN,
    MONTSERRAT_SEMIBOLD,
    MONTSERRAT_LIGHT,
    MONTSERRAT_REGULAR
  };

  enum class CustomTheme {
    BLACK = 0,
    CLOUDY,
    COCONUT,
    NUM_ITEMS
  };

  // for toolbar
  CustomTheme& operator++(CustomTheme& lhs);

} // namespace GUI
#endif  // IMGUI_DISABLE

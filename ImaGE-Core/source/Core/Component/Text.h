#pragma once

#include <string>
#include <glm/vec4.hpp>
#include "Color.h"

namespace Component {
  struct Text {
    Text() : textContent{ "YOUR TEXT HERE" }, fontName { "Arial" }, 
      color{ Color::COLOR_BLACK }, scale{ 1.f } {}
    Text(std::string const& textContent) : textContent{ textContent },
      fontName{ "Arial" }, color{ Color::COLOR_BLACK }, scale{ 1.f } {}

    inline void Clear() noexcept {
      textContent = "YOUR TEXT HERE";
      fontName = "Arial";
      scale = 1.f;
      color = Color::COLOR_BLACK;
    }

    std::string textContent;
    std::string fontName;
    float scale;
    glm::vec4 color;
  };
} // namespace Component
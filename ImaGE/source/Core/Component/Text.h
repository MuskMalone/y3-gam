#pragma once

#include <string>
#include <glm/vec4.hpp>
#include "Color.h"

namespace Component {
  struct Text {
    Text() : textContent{ "YOUR TEXT HERE" }, fontName { "Arial" }, 
      colour{ Color::COLOR_BLACK }, scale{ 1.f } {}
    Text(std::string const& textContent) : textContent{ textContent },
      fontName{ "Arial" }, colour{ Color::COLOR_BLACK }, scale{ 1.f } {}

    inline void Clear() noexcept {
      textContent = "YOUR TEXT HERE";
      fontName = "Arial";
      scale = 1.f;
      colour = Color::COLOR_BLACK;
    }

    std::string textContent;
    std::string fontName;
    float scale;
    glm::vec4 colour;
  };
} // namespace Component
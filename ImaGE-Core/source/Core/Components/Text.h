#pragma once
#include <string>
#include <glm/vec4.hpp>
#include <Asset/IGEAssets.h>
#include "Color.h"

namespace Component {
  struct Text {
    Text() : textContent{ "YOUR TEXT HERE" }, color{ Color::COLOR_BLACK }, 
      scale{ 1.f }, textAsset{}, fontFamilyName{ "None" } {}

    Text(IGE::Assets::GUID const& textAsset, std::string const& textContent, std::string const& fontFamilyName) : 
      textContent{ textContent }, color{ Color::COLOR_BLACK }, scale{ 1.f }, 
      textAsset{ textAsset }, fontFamilyName{ fontFamilyName } {}

    inline void Clear() noexcept {
      textContent = "YOUR TEXT HERE";
      scale = 1.f;
      color = Color::COLOR_BLACK;
      textAsset = {};
      fontFamilyName = { "None" };
    }

    std::string textContent;
    glm::vec4 color;
    float scale;
    IGE::Assets::GUID textAsset;
    std::string fontFamilyName;
  };
} // namespace Component
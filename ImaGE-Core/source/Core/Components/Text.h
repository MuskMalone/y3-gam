#pragma once
#include <string>
#include <glm/vec4.hpp>
#include <Asset/IGEAssets.h>
#include "Color.h"

namespace Component {

  struct Text {
    enum Alignment {
      LEFT,
      RIGHT,
      CENTER
    };

    Text() : textContent{ "YOUR TEXT HERE" }, color{ Color::COLOR_BLACK }, 
      scale{ 0.01f }, textAsset{}, fontFamilyName{ "None" }, alignment{ Alignment::LEFT }, multiLineSpacingOffset{ 0.f },
      textBoxEnabledFlag{ false }, textBoxDimensions{ 5.f, 5.f, 0.f }, newLineIndices {} {}

    Text(IGE::Assets::GUID const& textAsset, std::string const& textContent, std::string const& fontFamilyName) : 
      textContent{ textContent }, color{ Color::COLOR_BLACK }, scale{ 0.01f },
      textAsset{ textAsset }, fontFamilyName{ fontFamilyName }, alignment{ Alignment::LEFT }, multiLineSpacingOffset{ 0.f },
      textBoxEnabledFlag{ false }, textBoxDimensions{ 5.f, 5.f, 0.f }, newLineIndices{} {}

    inline void Clear() noexcept {
      textContent = "YOUR TEXT HERE";
      scale = 0.01f;
      color = Color::COLOR_BLACK;
      textAsset = {};
      fontFamilyName = { "None" };
      alignment = Alignment::LEFT;
      multiLineSpacingOffset = 0.f;
      textBoxEnabledFlag = false;
      textBoxDimensions = { 5.f, 5.f, 0.f };

      newLineIndices = {};
      newLineIndicesUpdatedFlag = false;
    }

    std::string textContent;
    glm::vec4 color;
    float scale;
    IGE::Assets::GUID textAsset;
    std::string fontFamilyName;
    int alignment;
    float multiLineSpacingOffset;
    bool textBoxEnabledFlag;
    glm::vec3 textBoxDimensions; // Width * Height

    // The following are not to be serialized:
    std::vector<std::pair<size_t, float>> newLineIndices;
    bool newLineIndicesUpdatedFlag{ false }; // Should be set to false whenever textContent is edited
  };

} // namespace Component
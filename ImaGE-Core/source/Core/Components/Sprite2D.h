#pragma once

namespace Component
{
  struct Sprite2D
  {
    // Default constructor to initialize color to white and texture to an invalid GUID
    Sprite2D() : color{ Color::COLOR_WHITE }, textureAsset{} {}
    Sprite2D(IGE::Assets::GUID guid) : color{ Color::COLOR_WHITE }, textureAsset{ guid } {}

    /*!*********************************************************************
    \brief
      Resets the component to default values.
    ************************************************************************/
    inline void Clear() noexcept {
      color = glm::vec4(1.0f);
      textureAsset = {};
    }

    glm::vec4 color;       // Color tint for the image (RGBA)
    IGE::Assets::GUID textureAsset;  // Texture GUID for the image asset
  };

} // namespace Component

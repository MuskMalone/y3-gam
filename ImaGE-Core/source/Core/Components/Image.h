#pragma once
/*!*********************************************************************
\file   Image.h
\author
\date   1-November-2024
\brief  Definition of the Image component, for UI elements like icons, backgrounds, and images.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/

#include <glm/glm.hpp>       // For color (glm::vec4)
#include <Asset/IGEAssets.h>
#include "Color.h"

namespace Component
{
    struct Image
    {
        // Default constructor to initialize color to white and texture to an invalid GUID
        Image() : color{ Color::COLOR_WHITE }, textureAsset{} {}

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


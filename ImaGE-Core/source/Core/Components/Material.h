/*!*********************************************************************
\file   Material.h
\author 
\date   5-October-2024
\brief  Definition of Material component, which determines the texture
        for the object when it is rendered.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Color.h"
#include "Graphics/Material.h"

namespace Component {
    struct Material {
      Material() {
        std::shared_ptr<Graphics::Texture> debugAlbedoTex = std::make_shared<Graphics::Texture>(2, 2);
        unsigned int debugAlbedoData[4] = {
            0xffff00ff, // Bright magenta (ABGR)
            0xffffff00, // Cyan (to create contrast for checkerboard)
            0xffffff00, // Cyan
            0xffff00ff  // Bright magenta
        };
        debugAlbedoTex->SetData(debugAlbedoData);

        std::shared_ptr<Graphics::Shader> shader = std::make_shared<Graphics::Shader>("../Assets/Shaders/Default.vert.glsl", "../Assets/Shaders/Default.frag.glsl");
        std::shared_ptr<Graphics::MaterialSource> materialSource0 = Graphics::MaterialSource::Create(shader);
        materialSource0->SetAlbedoMap(debugAlbedoTex);

        material = std::make_shared<Graphics::Material>(materialSource0);
      }
      Material(std::shared_ptr<Graphics::Material> material) : material{ material } {} // Constructor to set material instance

      inline void Clear() noexcept {
          material.reset();
      }

      std::shared_ptr<Graphics::Material> material;
    };
}
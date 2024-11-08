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
      Material() = default;
      Material(std::shared_ptr<Graphics::Material> material) : material{ material } {} // Constructor to set material instance

      //Material(Material const& rhs) : material{ Graphics::Material::Create(rhs.material->GetShader()) }, matIdx{rhs.matIdx} {}

      /*!*********************************************************************
      \brief
        Resets the material to default values
      ************************************************************************/
      inline void Clear() noexcept {
        material.reset();
        matIdx = 0;
      }

      std::shared_ptr<Graphics::Material> material;
      uint32_t matIdx = 0;
    };
}
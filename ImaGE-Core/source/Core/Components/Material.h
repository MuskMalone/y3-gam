#pragma once
#include "Color.h"
#include "Graphics/Material.h"

namespace Component {
    struct Material {
        Material() = default;
        Material(std::shared_ptr<Graphics::Material> material) : material{ material } {} // Constructor to set material instance

        inline void Clear() noexcept {
            material.reset();
        }

        std::shared_ptr<Graphics::Material> material;
    };
}
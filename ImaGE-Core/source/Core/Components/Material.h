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
#include <Asset/IGEAssets.h>
#include "Graphics/MaterialTable.h"

namespace Component {
    struct Material {
        Material() = default;

        Material(IGE::Assets::GUID guid)
            : materialGUID(guid),
            matIdx(Graphics::MaterialTable::AddMaterialByGUID(guid))
        {}

        // Clear method to reset the component to a default material
        inline void Clear() noexcept {
            materialGUID = IGE::Assets::GUID{};  // Clear GUID
            matIdx = 0;                          // Reset to default material
        }

        // Getters and Setters for GUID
        inline IGE::Assets::GUID GetGUID() const { return materialGUID; }

        inline void SetGUID(const IGE::Assets::GUID& guid) {
            materialGUID = guid;
            matIdx = Graphics::MaterialTable::AddMaterialByGUID(guid); // Update matIdx
        }

        // Material GUID and matIdx for instancing
        IGE::Assets::GUID materialGUID{};
        uint32_t matIdx = 0;
    };
}

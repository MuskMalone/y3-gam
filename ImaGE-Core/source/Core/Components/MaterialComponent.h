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

namespace Graphics {
    class Material;
    class ShaderLibrary;
}

namespace Component {
    struct MaterialComponent {
        MaterialComponent();

        /*!*********************************************************************
        \brief Resets the material to default values.
        ************************************************************************/
        void Clear() noexcept;

        const std::string& GetShaderName() const;
        void SetShaderName(const std::string& name);

        glm::vec3 GetAlbedoColor() const;
        void SetAlbedoColor(const glm::vec3& color);

        float GetMetalness() const;
        void SetMetalness(float value);

        float GetRoughness() const;
        void SetRoughness(float value);

        float GetAO() const;
        void SetAO(float aoValue);

        float GetEmission() const;
        void SetEmission(float value);

        float GetTransparency() const;
        void SetTransparency(float value);

        glm::vec2 GetTiling() const;
        void SetTiling(const glm::vec2& tilingValue);

        glm::vec2 GetOffset() const;
        void SetOffset(const glm::vec2& offsetValue);

        IGE::Assets::GUID GetAlbedoMap() const;
        void SetAlbedoMap(const IGE::Assets::GUID& texture);

        IGE::Assets::GUID GetNormalMap() const;
        void SetNormalMap(const IGE::Assets::GUID& texture);

        IGE::Assets::GUID GetMetalnessMap() const;
        void SetMetalnessMap(const IGE::Assets::GUID& texture);

        IGE::Assets::GUID GetRoughnessMap() const;
        void SetRoughnessMap(const IGE::Assets::GUID& texture);

        uint32_t matIdx = 0;
        std::string shaderName;
        glm::vec3 albedoColor{ 1.0f, 1.0f, 1.0f };
        float metalness{ 0.0f };
        float roughness{ 0.5f };
        float ao{ 1.0f };
        float emission{ 0.0f };
        float transparency{ 1.0f };
        uint32_t flags{ 0 };
        glm::vec2 tiling{ 1.0f, 1.0f };
        glm::vec2 offset{ 0.0f, 0.0f };

        // Texture GUIDs for material maps
        IGE::Assets::GUID albedoMap{};
        IGE::Assets::GUID normalMap{};
        IGE::Assets::GUID metalnessMap{};
        IGE::Assets::GUID roughnessMap{};
    };
}

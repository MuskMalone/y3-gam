/*!*********************************************************************
\file   Material.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Material class allows for managing and applying material properties such as albedo color, metalness,
        roughness, transparency, and emission. It also supports custom textures (albedo, normal, metalness, 
        and roughness) per instance, allowing for overrides of the default material source properties.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "MaterialSource.h"
#include "Renderer.h"
namespace Graphics{
    class Material {

    public:
        // Constructor that takes a reference to a MaterialSource
        explicit Material(std::shared_ptr<MaterialSource> materialSource)
            : mMaterialSource{ materialSource }, mAlbedoColor{ materialSource->GetDefaultAlbedoColor() },
            mMetalness{ materialSource->GetDefaultMetalness() }, mRoughness{ materialSource->GetDefaultRoughness() }, mAO{ materialSource->GetDefaultAO() },
            mEmission{ materialSource->GetDefaultEmission() }, mTransparency{ materialSource->GetDefaultTransparency() },
            mUseCustomAlbedoMap{ false }, mUseCustomNormalMap{ false }, mUseCustomMetalnessMap{ false }, mUseCustomRoughnessMap{ false }
        {}

        // Setters and getters for per-instance properties
       glm::vec3 GetAlbedoColor() const { return mAlbedoColor; }
        void SetAlbedoColor(const glm::vec3& color) { mAlbedoColor = color; }

        float GetMetalness() const { return mMetalness; }
        void SetMetalness(float value) { mMetalness = value; }

        float GetRoughness() const { return mRoughness; }
        void SetRoughness(float value) { mRoughness = value; }

        float GetEmission() const { return mEmission; }
        void SetEmission(float value) { mEmission = value; }

        float GetTransparency() const { return mTransparency; }
        void SetTransparency(float value) { mTransparency = value; }

        // Texture Management
        IGE::Assets::GUID GetAlbedoMap() const { return mUseCustomAlbedoMap ? mCustomAlbedoMap : mMaterialSource->GetAlbedoMap(); }
        void SetAlbedoMap(IGE::Assets::GUID texture) { mCustomAlbedoMap = texture; mUseCustomAlbedoMap = true; }
        void ClearAlbedoMap() { mUseCustomAlbedoMap = false; }

        IGE::Assets::GUID GetNormalMap() const { return mUseCustomNormalMap ? mCustomNormalMap : mMaterialSource->GetNormalMap(); }
        void SetNormalMap(IGE::Assets::GUID texture) { mCustomNormalMap = texture; mUseCustomNormalMap = true; }
        void ClearNormalMap() { mUseCustomNormalMap = false; }

        IGE::Assets::GUID GetMetalnessMap() const { return mUseCustomMetalnessMap ? mCustomMetalnessMap : mMaterialSource->GetMetalnessMap(); }
        void SetMetalnessMap(IGE::Assets::GUID texture) { mCustomMetalnessMap = texture; mUseCustomMetalnessMap = true; }
        void ClearMetalnessMap() { mUseCustomMetalnessMap = false; }

        IGE::Assets::GUID GetRoughnessMap() const { return mUseCustomRoughnessMap ? mCustomRoughnessMap : mMaterialSource->GetRoughnessMap(); }
        void SetRoughnessMap(IGE::Assets::GUID texture) { mCustomRoughnessMap = texture; mUseCustomRoughnessMap = true; }
        void ClearRoughnessMap() { mUseCustomRoughnessMap = false; }

        // Apply Material Properties to Shader
        void Apply(std::shared_ptr<Shader> shader) const {
            // Apply instance-specific overrides
            shader->SetUniform("u_Albedo", mAlbedoColor);
            shader->SetUniform("u_Metalness", mMetalness);
            shader->SetUniform("u_Roughness", mRoughness);
            shader->SetUniform("u_Transparency", mTransparency);
            shader->SetUniform("u_AO", 1.f);
            //shader->SetUniform("u_Emission", mEmission);


            // Apply textures (use instance override if available)
            shader->SetUniform("u_AlbedoMap", GetAlbedoMap() ? GetAlbedoMap() : Renderer::GetWhiteTexture(), 0);

            //if (auto normalMap = GetNormalMap()) shader->SetUniform("u_NormalMap", normalMap);
            //if (auto metalnessMap = GetMetalnessMap()) shader->SetUniform("u_MetalnessMap", metalnessMap);
            //if (auto roughnessMap = GetRoughnessMap()) shader->SetUniform("u_RoughnessMap", roughnessMap);
        }

        // Get the underlying material source
        std::shared_ptr<MaterialSource> GetMaterialSource() const { return mMaterialSource; }
    private:
        // Reference to the shared material source
        std::shared_ptr<MaterialSource> mMaterialSource;

        // Per-instance overrides
        glm::vec3 mAlbedoColor;
        float mMetalness;
        float mRoughness;
        float mAO;
        float mEmission;
        float mTransparency;

        // Texture overrides
        //std::shared_ptr<Texture> 
        IGE::Assets::GUID mCustomAlbedoMap;
        IGE::Assets::GUID mCustomNormalMap;
        IGE::Assets::GUID mCustomMetalnessMap;
        IGE::Assets::GUID mCustomRoughnessMap;

        // Flags for texture use
        bool mUseCustomAlbedoMap;
        bool mUseCustomNormalMap;
        bool mUseCustomMetalnessMap;
        bool mUseCustomRoughnessMap;
    };

}
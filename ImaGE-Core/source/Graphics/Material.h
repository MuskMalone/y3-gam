#pragma once
#include "MaterialSource.h"

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
        std::shared_ptr<Texture> GetAlbedoMap() const { return mUseCustomAlbedoMap ? mCustomAlbedoMap : mMaterialSource->GetAlbedoMap(); }
        void SetAlbedoMap(std::shared_ptr<Texture> texture) { mCustomAlbedoMap = texture; mUseCustomAlbedoMap = true; }
        void ClearAlbedoMap() { mUseCustomAlbedoMap = false; }

        std::shared_ptr<Texture> GetNormalMap() const { return mUseCustomNormalMap ? mCustomNormalMap : mMaterialSource->GetNormalMap(); }
        void SetNormalMap(std::shared_ptr<Texture> texture) { mCustomNormalMap = texture; mUseCustomNormalMap = true; }
        void ClearNormalMap() { mUseCustomNormalMap = false; }

        std::shared_ptr<Texture> GetMetalnessMap() const { return mUseCustomMetalnessMap ? mCustomMetalnessMap : mMaterialSource->GetMetalnessMap(); }
        void SetMetalnessMap(std::shared_ptr<Texture> texture) { mCustomMetalnessMap = texture; mUseCustomMetalnessMap = true; }
        void ClearMetalnessMap() { mUseCustomMetalnessMap = false; }

        std::shared_ptr<Texture> GetRoughnessMap() const { return mUseCustomRoughnessMap ? mCustomRoughnessMap : mMaterialSource->GetRoughnessMap(); }
        void SetRoughnessMap(std::shared_ptr<Texture> texture) { mCustomRoughnessMap = texture; mUseCustomRoughnessMap = true; }
        void ClearRoughnessMap() { mUseCustomRoughnessMap = false; }

        // Apply Material Properties to Shader
        void Apply(std::shared_ptr<Shader> shader) const {
            // Apply instance-specific overrides
            shader->SetUniform("u_AlbedoColor", mAlbedoColor);
            shader->SetUniform("u_Metalness", mMetalness);
            shader->SetUniform("u_Roughness", mRoughness);
            shader->SetUniform("u_Emission", mEmission);
            shader->SetUniform("u_Transparency", mTransparency);

            //// Apply textures (use instance override if available)
            //if (auto albedoMap = GetAlbedoMap()) shader->SetUniform("u_AlbedoMap", albedoMap);
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
        std::shared_ptr<Texture> mCustomAlbedoMap;
        std::shared_ptr<Texture> mCustomNormalMap;
        std::shared_ptr<Texture> mCustomMetalnessMap;
        std::shared_ptr<Texture> mCustomRoughnessMap;

        // Flags for texture use
        bool mUseCustomAlbedoMap;
        bool mUseCustomNormalMap;
        bool mUseCustomMetalnessMap;
        bool mUseCustomRoughnessMap;
    };

}
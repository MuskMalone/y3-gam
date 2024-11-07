/*!*********************************************************************
\file   MaterialData.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The MaterialData class allows for managing and applying material properties such as albedo color, metalness,
        roughness, transparency, and emission. It also supports custom textures (albedo, normal, metalness, 
        and roughness) per instance, allowing for overrides of the default material source properties.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Renderer.h"
#include "Asset/IGEAssets.h"
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>

namespace Graphics {

    enum class MaterialDataFlag {
        NONE = 0,
        DEPTH_TEST = 1 << 0,
        BLEND = 1 << 1,
        TWO_SIDED = 1 << 2,
        DISABLE_SHADOW_CASTING = 1 << 3
    };

    class MaterialData {
    public:
        MaterialData(std::shared_ptr<Shader> shader)
            : mShader(std::move(shader)),       // Initialize the shader with the passed-in shared_ptr
            mAlbedoColor(1.0f, 1.0f, 1.0f),   // Default white albedo color
            mMetalness(0.0f),                 // Default non-metallic
            mRoughness(0.5f),                 // Default roughness value
            mAO(1.0f),                        // Default ambient occlusion
            mEmission(0.0f),                  // Default no emission
            mTransparency(1.0f),              // Default fully opaque
            mFlags(0),                        // No flags set
            mTiling(1.0f, 1.0f),              // Default tiling factor
            mOffset(0.0f, 0.0f)               // Default offset
        {}

        static std::shared_ptr<MaterialData> Create(std::shared_ptr<Shader> shader) {
            return std::make_shared<MaterialData>(shader);
        }

        // Shader access
        inline std::shared_ptr<Shader> GetShader() const { return mShader; }

        // Property Getters and Setters
        glm::vec3 GetAlbedoColor() const { return mAlbedoColor; }
        void SetAlbedoColor(const glm::vec3& color) { mAlbedoColor = color; }

        float GetMetalness() const { return mMetalness; }
        void SetMetalness(float value) { mMetalness = value; }

        float GetRoughness() const { return mRoughness; }
        void SetRoughness(float value) { mRoughness = value; }

        float GetAO() const { return mAO; }
        void SetAO(float ao) { mAO = ao; }

        float GetEmission() const { return mEmission; }
        void SetEmission(float value) { mEmission = value; }

        float GetTransparency() const { return mTransparency; }
        void SetTransparency(float value) { mTransparency = value; }

        glm::vec2 GetTiling() const { return mTiling; }
        void SetTiling(const glm::vec2& tiling) { mTiling = tiling; }

        glm::vec2 GetOffset() const { return mOffset; }
        void SetOffset(const glm::vec2& offset) { mOffset = offset; }

        // Texture Getters and Setters
        IGE::Assets::GUID GetAlbedoMap() const;
        void SetAlbedoMap(IGE::Assets::GUID const& texture);

        IGE::Assets::GUID GetNormalMap() const;
        void SetNormalMap(IGE::Assets::GUID const& texture);

        IGE::Assets::GUID GetMetalnessMap() const;
        void SetMetalnessMap(IGE::Assets::GUID const& texture);

        IGE::Assets::GUID GetRoughnessMap() const;
        void SetRoughnessMap(IGE::Assets::GUID const& texture);

        // Apply material to shader
        void Apply(std::shared_ptr<Shader> shader) const;

        // Flags handling
        bool GetFlag(MaterialDataFlag flag) const { return mFlags & static_cast<uint32_t>(flag); }
        void SetFlag(MaterialDataFlag flag, bool value) {
            if (value) mFlags |= static_cast<uint32_t>(flag);
            else mFlags &= ~static_cast<uint32_t>(flag);
        }

    private:
        std::shared_ptr<Shader> mShader;

        glm::vec3 mAlbedoColor;
        float mMetalness;
        float mRoughness;
        float mAO;
        float mEmission;
        float mTransparency;

        uint32_t mFlags;

        glm::vec2 mTiling{ 1.0f, 1.0f }; // Default tiling (1,1) - no repetition
        glm::vec2 mOffset{ 0.0f, 0.0f }; // Default offset (0,0) - no shift

        // Texture GUIDs
        IGE::Assets::GUID mAlbedoMap{};
        IGE::Assets::GUID mNormalMap{};
        IGE::Assets::GUID mMetalnessMap{};
        IGE::Assets::GUID mRoughnessMap{};
    };

}

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
#include <Asset/SmartPointer.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <rttr/registration_friend>

namespace Graphics {

    enum class MaterialDataFlag {
        NONE = 0,
        DEPTH_TEST = 1 << 0,
        BLEND = 1 << 1,
        TWO_SIDED = 1 << 2,
        DISABLE_SHADOW_CASTING = 1 << 3
    };

    struct MaterialProperties {
        glm::vec2 Tiling;
        glm::vec2 Offset;
        glm::vec4 AlbedoColor;  // Base color
        float Metalness;        // Metalness factor
        float Roughness;        // Roughness factor
        float Transparency;     // Transparency (alpha)
        float AO;               // Ambient occlusion
        glm::vec4 Emission;
    };

    class MaterialData {
    public:
        MaterialData() = default;
        MaterialData(std::shared_ptr<Shader> shader, std::string const& shaderName, std::string const& matName = "unknown")
            : mShader(std::move(shader)),
            mName(matName),
            mShaderName(shaderName),// Initialize the shader with the passed-in shared_ptr
            mAlbedoColor(1.0f, 1.0f, 1.0f),   // Default white albedo color
            mMetalness(0.0f),                 // Default non-metallic
            mRoughness(0.5f),                 // Default roughness value
            mAO(1.0f),                        // Default ambient occlusion
            mEmission(0.f,0.f,0.f,0.0f),                  // Default no emission
            mTransparency(1.0f),              // Default fully opaque
            mFlags(0),                        // No flags set
            mTiling(1.0f, 1.0f),              // Default tiling factor
            mOffset(0.0f, 0.0f)               // Default offset
        {}

        static std::shared_ptr<MaterialData> Create(std::shared_ptr<Shader> shader, std::string const& shaderName, std::string const& matName ="unknown") {
            return std::make_shared<MaterialData>(shader, shaderName, matName);
        }

        static std::shared_ptr<MaterialData> Create(std::string const& shaderName, std::string const& matName = "unknown") {
            return Create(ShaderLibrary::Get(shaderName), shaderName, matName);
        }

        // Shader access
        inline std::shared_ptr<Shader> GetShader() const { return mShader; }
        inline void SetShader(std::string const& shaderName) { mShader = ShaderLibrary::Get(shaderName); mModified = true; }

        // Property Getters and Setters
        std::string const& GetName() { return mName; };
        void SetName(std::string const& name) { mName = name; }

        std::string const& GetShaderName() { return mShaderName; }
        void SetShaderName(std::string const& name) { mShaderName = name; SetShader(name); }

        glm::vec3 const& GetAlbedoColor() const { return mAlbedoColor; }
        void SetAlbedoColor(const glm::vec3& color) { mAlbedoColor = color; mModified = true;}

        float GetMetalness() const { return mMetalness; }
        void SetMetalness(float value) { mMetalness = value; mModified = true;}

        float GetRoughness() const { return mRoughness; }
        void SetRoughness(float value) { mRoughness = value; mModified = true;}

        float GetAO() const { return mAO; }
        void SetAO(float ao) { mAO = ao; mModified = true; }

        glm::vec4 GetEmission() const { return mEmission; }
        void SetEmission(glm::vec4 value) { mEmission = value; mModified = true; }

        float GetTransparency() const { return mTransparency; }
        void SetTransparency(float value) { mTransparency = value; mModified = true; }

        glm::vec2 const& GetTiling() const { return mTiling; }
        void SetTiling(const glm::vec2& tiling) { mTiling = tiling; mModified = true;}

        glm::vec2 const& GetOffset() const { return mOffset; }
        void SetOffset(const glm::vec2& offset) { mOffset = offset; mModified = true; }

        // Texture Getters and Setters
        bool IsDefaultAlbedoMap() const { return !mAlbedoMap.IsValid(); }
        bool IsDefaultNormalMap() const { return !mNormalMap.IsValid(); }
        bool IsDefaultMetalnessMap() const { return !mMetalnessMap.IsValid(); }
        bool IsDefaultRoughnessMap() const { return !mRoughnessMap.IsValid(); }
    
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

        bool IsModified() const { return mModified; }
        void ClearModifiedFlag() { mModified = false; }

    private:
        std::shared_ptr<Shader> mShader;
        std::string mName;
        std::string mShaderName;
        glm::vec3 mAlbedoColor;
        float mMetalness;
        float mRoughness;
        float mAO;
        glm::vec4 mEmission;
        float mTransparency;

        bool mModified;

        uint32_t mFlags;

        glm::vec2 mTiling{ 1.0f, 1.0f }; // Default tiling (1,1) - no repetition
        glm::vec2 mOffset{ 0.0f, 0.0f }; // Default offset (0,0) - no shift

        // Texture GUIDs
        IGE::Assets::GUID mAlbedoMap{};
        IGE::Assets::GUID mNormalMap{};
        IGE::Assets::GUID mMetalnessMap{};
        IGE::Assets::GUID mRoughnessMap{};

        RTTR_REGISTRATION_FRIEND  // to allow rttr to read private members
    };

}

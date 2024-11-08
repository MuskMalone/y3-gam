#include "pch.h"
#include "MaterialData.h"
#include "Renderer.h"

namespace Graphics {
    // Apply material to shader


    // Texture Getters and Setters

    IGE::Assets::GUID MaterialData::GetAlbedoMap() const { return mAlbedoMap ? mAlbedoMap : Renderer::GetWhiteTexture(); }

    void MaterialData::SetAlbedoMap(IGE::Assets::GUID const& texture) { mAlbedoMap = texture; }

    IGE::Assets::GUID MaterialData::GetNormalMap() const { return mNormalMap ? mNormalMap : Renderer::GetWhiteTexture(); }

    void MaterialData::SetNormalMap(IGE::Assets::GUID const& texture) { mNormalMap = texture; }

    IGE::Assets::GUID MaterialData::GetMetalnessMap() const { return mMetalnessMap; }

    void MaterialData::SetMetalnessMap(IGE::Assets::GUID const& texture) { mMetalnessMap = texture; }

    IGE::Assets::GUID MaterialData::GetRoughnessMap() const { return mRoughnessMap; }

    void MaterialData::SetRoughnessMap(IGE::Assets::GUID const& texture) { mRoughnessMap = texture; }
 
    void MaterialData::Apply(std::shared_ptr<Shader> shader) const {
        shader->SetUniform("u_Tiling", mTiling);
        shader->SetUniform("u_Offset", mOffset);
        shader->SetUniform("u_Albedo", mAlbedoColor);
        shader->SetUniform("u_Metalness", mMetalness);
        shader->SetUniform("u_Roughness", mRoughness);
        shader->SetUniform("u_Transparency", mTransparency);
        shader->SetUniform("u_AO", mAO);
       // shader->SetUniform("u_Emission", mEmission);
    }
}
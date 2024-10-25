#include "pch.h"
#include "Material.h"
#include "Renderer.h"

namespace Graphics {
    // Apply material to shader


    // Texture Getters and Setters

    IGE::Assets::GUID Material::GetAlbedoMap() const { return mAlbedoMap ? mAlbedoMap : Renderer::GetWhiteTexture(); }

    void Material::SetAlbedoMap(IGE::Assets::GUID const& texture) { mAlbedoMap = texture; }

    IGE::Assets::GUID Material::GetNormalMap() const { return mNormalMap ? mNormalMap : Renderer::GetWhiteTexture(); }

    void Material::SetNormalMap(IGE::Assets::GUID const& texture) { mNormalMap = texture; }

    IGE::Assets::GUID Material::GetMetalnessMap() const { return mMetalnessMap; }

    void Material::SetMetalnessMap(IGE::Assets::GUID const& texture) { mMetalnessMap = texture; }

    IGE::Assets::GUID Material::GetRoughnessMap() const { return mRoughnessMap; }

    void Material::SetRoughnessMap(IGE::Assets::GUID const& texture) { mRoughnessMap = texture; }
 
    void Material::Apply(std::shared_ptr<Shader> shader) const {
        shader->SetUniform("u_Albedo", mAlbedoColor);
        shader->SetUniform("u_Metalness", mMetalness);
        shader->SetUniform("u_Roughness", mRoughness);
        shader->SetUniform("u_Transparency", mTransparency);
        shader->SetUniform("u_AO", mAO);
       // shader->SetUniform("u_Emission", mEmission);
    }
}
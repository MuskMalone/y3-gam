#include "pch.h"
#include "MaterialTable.h"
#include "MaterialData.h"

namespace Graphics {
    std::vector<std::shared_ptr<MaterialData>> MaterialTable::mMaterials;
    // Add a material to the table and return its index
    uint32_t Graphics::MaterialTable::AddMaterial(std::shared_ptr<Graphics::MaterialData>& material) {
        mMaterials.push_back(material);
        return static_cast<uint32_t>(mMaterials.size() - 1);
    }

    // Retrieve material by index

    std::shared_ptr<MaterialData> MaterialTable::GetMaterial(uint32_t index = 0) {
        return mMaterials[index];
    }

    // Bind textures for all materials to the shader

    void Graphics::MaterialTable::ApplyMaterialTextures(std::shared_ptr<Shader>& shader) {
        size_t size = mMaterials.size();
        if (size <= 1) return;  // Exit if there's only the default material

        // Bind default textures once to specific units
        //int defaultAlbedoUnit = 0;   // Unit 0 for default albedo texture
        //int defaultNormalUnit = 16;  // Unit 16 for default normal map

        // Bind the default textures only once @TODO CHANGE THIS TO SOMEWHERE ELSE
        int const defaultAlbedoUnit{ static_cast<int>(IGE_REF(IGE::Assets::TextureAsset, Renderer::GetWhiteTexture())->mTexture.Bind()) };
        int const defaultNormalUnit{ static_cast<int>(IGE_REF(IGE::Assets::TextureAsset, Renderer::GetWhiteTexture())->mTexture.Bind()) }; // cahnge to normal Tex

        // Initialize texture unit arrays to default values
        std::vector<int> albedoTextureUnits(size, defaultAlbedoUnit);
        std::vector<int> normalTextureUnits(size, defaultNormalUnit);

        // Start from index 1 to skip the default material
        for (uint32_t i = 1; i < mMaterials.size() && i < 16; ++i) {  // Up to 16 unique textures
            std::shared_ptr<MaterialData> const& material = mMaterials[i];

            // Get the material’s textures
            auto albedoMap = material->GetAlbedoMap();
            auto normalMap = material->GetNormalMap();

            // Only bind the albedo map if it’s unique (not the default texture)
            if (albedoMap != Renderer::GetWhiteTexture()) {
              int const texUnit{ static_cast<int>(IGE_REF(IGE::Assets::TextureAsset, albedoMap)->mTexture.Bind()) };
              albedoTextureUnits[i] = texUnit;  // Assign this unique texture unit to the shader array
            }

            // Only bind the normal map if it’s unique (not the default texture)
            if (normalMap != Renderer::GetWhiteTexture()) { // @TODO Change to normal Tex
              int const texUnit{ static_cast<int>(IGE_REF(IGE::Assets::TextureAsset, normalMap)->mTexture.Bind()) };
              normalTextureUnits[i] = texUnit; 
            }
        }

        // Set texture unit arrays in the shader; any unused slots will point to default textures
        shader->SetUniform("u_AlbedoMaps", albedoTextureUnits.data(), static_cast<unsigned>(albedoTextureUnits.size()));
        shader->SetUniform("u_NormalMaps", normalTextureUnits.data(), static_cast<unsigned>(normalTextureUnits.size()));
    }
    void MaterialTable::ClearMaterials(){
        if (mMaterials.size() > 1) {
            mMaterials.erase(mMaterials.begin() + 1, mMaterials.end());
        }
    }
}
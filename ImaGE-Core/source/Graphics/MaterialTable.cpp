#include "pch.h"
#include "MaterialTable.h"
#include "Material.h"

std::vector<std::shared_ptr<Graphics::Material>> Graphics::MaterialTable::mMaterials;
// Add a material to the table and return its index
 uint32_t Graphics::MaterialTable::AddMaterial(std::shared_ptr<Graphics::Material>& material) {
    mMaterials.push_back(material);
    return static_cast<uint32_t>(mMaterials.size() - 1);
}

// Retrieve material by index

std::shared_ptr<Graphics::Material> Graphics::MaterialTable::GetMaterial(uint32_t index){
    return mMaterials[index];
}

// Bind textures for all materials to the shader

void Graphics::MaterialTable::ApplyMaterialTextures(std::shared_ptr<Graphics::Shader>& shader){
    // Use vectors to store the texture unit indices dynamically
    size_t size = mMaterials.size();
    if (!size) return;
    std::vector<int> albedoTextureUnits;
    std::vector<int> normalTextureUnits;

    // Resize the vectors to match the number of materials (or up to 16 as per your original logic)
    albedoTextureUnits.reserve(size);
    normalTextureUnits.reserve(size);

    for (uint32_t i = 0; i < mMaterials.size() && i < 16; ++i) {  // Up to 16 textures
        std::shared_ptr<Graphics::Material> const& material = mMaterials[i];

        // Bind Albedo and Normal maps to respective texture units
        IGE_REF(IGE::Assets::TextureAsset, material->GetAlbedoMap())->mTexture.Bind(i);
        albedoTextureUnits[i] = i;  // Store texture unit for shader uniform

        IGE_REF(IGE::Assets::TextureAsset, (material->GetNormalMap()))->mTexture.Bind(i + 16); // Bind to next 16 texture units
        normalTextureUnits[i] = i + 16;
    }

    // Set texture units in the shader using glUniform1iv, using vector data
    shader->SetUniform("u_AlbedoMap", albedoTextureUnits.data(), static_cast<unsigned>(albedoTextureUnits.size()));
    shader->SetUniform("u_NormalMap", normalTextureUnits.data(), static_cast<unsigned>(normalTextureUnits.size()));
}

#pragma once
#include "Asset/IGEAssets.h"
#include "Shader.h"

//TODO CHANGE TO GUID LATER
namespace Graphics {
    class Material;
    class MaterialTable {
    public:
        // Add a material to the table and return its index
        static uint32_t AddMaterial(std::shared_ptr<Material>& material);

        // Retrieve material by index
        static std::shared_ptr<Material> GetMaterial(uint32_t index);

        // Bind textures for all materials to the shader
        static void ApplyMaterialTextures(std::shared_ptr<Graphics::Shader>& shader);

    private:
        static std::vector<std::shared_ptr<Material>> mMaterials;
    };
}
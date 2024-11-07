#pragma once
#include "Asset/IGEAssets.h"
#include "Shader.h"

//TODO CHANGE TO GUID LATER
namespace Graphics {

    struct MatData {
        IGE::Assets::GUID guid;
        glm::vec3 albedoColor{ 1.0f, 1.0f, 1.0f };
        float metalness{ 0.0f };
        float roughness{ 0.5f };
        float ao{ 1.0f };
        float emission{ 0.0f };
        float transparency{ 1.0f };
        glm::vec2 tiling{ 1.0f, 1.0f };
        glm::vec2 offset{ 0.0f, 0.0f };

        // You can add more fields as needed, like maps for textures, flags, etc.
    };


    class MaterialData;
    class MaterialTable {
    public:
        // Add a material to the table and return its index
        static uint32_t AddMaterial(std::shared_ptr<MaterialData>& material);
        static uint32_t AddMaterialByGUID(IGE::Assets::GUID const& guid);

        // Retrieve material by index
        static std::shared_ptr<MaterialData> GetMaterial(uint32_t index);

        // Retrieve material by GUID
        static std::shared_ptr<MaterialData> GetMaterialByGUID(const IGE::Assets::GUID& guid);

        // Bind textures for all materials to the shader
        static void ApplyMaterialTextures(std::shared_ptr<Graphics::Shader>& shader, size_t batchStart, size_t batchEnd);
        static void ApplyMaterialTextures(std::shared_ptr<Graphics::Shader>& shader);

        static void SaveMaterials();
        static void LoadMaterials();

        static void ClearMaterials();

    private:
        static std::unordered_map<IGE::Assets::GUID, uint32_t> mGUIDToIndexMap;
        static std::vector<std::shared_ptr<MaterialData>> mMaterials;
    };
}
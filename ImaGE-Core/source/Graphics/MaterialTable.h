#pragma once
#include "Shader.h"

//TODO CHANGE TO GUID LATER
namespace Graphics {

    class MaterialData;
    class MaterialTable {
    public:
        // Add a material to the table and return its index
        static uint32_t AddMaterial(std::shared_ptr<MaterialData>& material);
        static uint32_t AddMaterialByGUID(IGE::Assets::GUID const& guid);

        static void DeleteMaterial(IGE::Assets::GUID const& guid);

        // Retrieve material by index
        static std::shared_ptr<MaterialData> const& GetMaterial(uint32_t index);

        // Retrieve material by GUID
        static uint32_t GetMaterialIndexByGUID(const IGE::Assets::GUID& guid);
        static std::shared_ptr<MaterialData> const& GetMaterialByGUID(const IGE::Assets::GUID& guid);

        // Bind textures for all materials to the shader
        static void ApplyMaterialTextures(std::shared_ptr<Graphics::Shader> const& shader, size_t batchStart, size_t batchEnd);
        static void ApplyMaterialTextures(std::shared_ptr<Graphics::Shader> const& shader);


        static IGE::Assets::GUID CreateAndImportMatFile(const std::string& name = "NewMaterial");
        static void SaveMaterial(IGE::Assets::GUID const& guid);
        static void SaveMaterials();
        static std::shared_ptr<MaterialData> LoadMaterial(std::string const& fp);

        static size_t GetMaterialCount() noexcept { return mMaterials.empty() ? 0 : mMaterials.size() - 1; }
        static void ClearMaterials();

        inline static constexpr unsigned sMaterialsPerBatch = 16 - 1;

    private:
        static std::unordered_map<IGE::Assets::GUID, uint32_t> mGUIDToIndexMap;
        static std::vector<std::shared_ptr<MaterialData>> mMaterials;
    };
}
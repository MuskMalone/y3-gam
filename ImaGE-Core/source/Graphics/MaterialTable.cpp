#include "pch.h"
#include "MaterialTable.h"
#include "MaterialData.h"
#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"

namespace Graphics {
    std::unordered_map<IGE::Assets::GUID, uint32_t> MaterialTable::mGUIDToIndexMap;
    std::vector<std::shared_ptr<MaterialData>> MaterialTable::mMaterials;
    // Add a material to the table and return its index
    uint32_t Graphics::MaterialTable::AddMaterial(std::shared_ptr<Graphics::MaterialData>& material) {
        mMaterials.push_back(material);
        return static_cast<uint32_t>(mMaterials.size() - 1);
    }

    uint32_t MaterialTable::AddMaterialByGUID(IGE::Assets::GUID const& guid){
        // Check if the material already exists
        auto it = mGUIDToIndexMap.find(guid);
        if (it != mGUIDToIndexMap.end()) {
            return it->second; // Return existing index
        }

        // Load the material data from the AssetManager
        //auto materialData = IGE::AssetManager::GetInstance().GetAsset<MaterialData>(guid);
        std::shared_ptr<MaterialData> materialData{}; //TODO change this
        if (!materialData) {
            // Handle missing material (use a default material if needed)
            materialData = MaterialData::Create(ShaderLibrary::Get("PBR")); //TODO CHNAGE THIS
        }

        // Add the new material to the table
        uint32_t newIndex = static_cast<uint32_t>(mMaterials.size());
        mMaterials.push_back(materialData);
        mGUIDToIndexMap[guid] = newIndex;

        return newIndex;
    }

    // Retrieve material by index

    std::shared_ptr<MaterialData> MaterialTable::GetMaterial(uint32_t index = 0) {
        return mMaterials[index];
    }

    std::shared_ptr<MaterialData> MaterialTable::GetMaterialByGUID(const IGE::Assets::GUID& guid){
        auto it = mGUIDToIndexMap.find(guid);
        if (it != mGUIDToIndexMap.end()) {
            return GetMaterial(it->second);
        }

        std::cerr << "Warning: Material with GUID " << " not found in MaterialTable.\n"; //@TODO CHANGE THIS TO DEBUGLOGGER
        Debug::DebugLogger::GetInstance().LogError("Material With GUID not found in MaterialTable");
        return nullptr;
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
    void MaterialTable::SaveMaterials() {
        for (size_t i = 1; i < mMaterials.size(); ++i) {  // Start from index 1 to skip the default material at index 0
            auto& material = mMaterials[i];
            if (!material) continue;

            // Create a unique filename for each material based on its GUID
            std::stringstream ss;
            ss << gMaterialDirectory << "mat" << i << ".mat";
           // std::string filename = "Materials/" + material->GetGUID().ToString() + ".mat";
            std::string filename = ss.str();

            // Prepare the metadata struct with material properties
            MatData data;
            //metadata.guid = material->GetGUID();
            data.albedoColor = material->GetAlbedoColor();
            data.metalness = material->GetMetalness();
            data.roughness = material->GetRoughness();
            data.ao = material->GetAO();
            data.emission = material->GetEmission();
            data.transparency = material->GetTransparency();
            data.tiling = material->GetTiling();
            data.offset = material->GetOffset();
            data.albedoMap = material->GetAlbedoMap();
            data.normalMap = material->GetNormalMap();
            data.metalnessMap = material->GetMetalnessMap();
            data.roughnessMap = material->GetRoughnessMap();

            // Serialize the metadata into the file
            Serialization::Serializer::SerializeAny(data, filename);
        }
    }


    void MaterialTable::LoadMaterials() {
        std::string directory = "Materials/";

        // Check if directory exists or handle error if it doesn't
        if (!std::filesystem::exists(directory)) {
            std::cerr << "Directory " << directory << " does not exist. No materials to load.\n";
            return;
        }

        // Iterate over each .mat file in the directory
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == ".mat") {
                // Deserialize the material data from the file
                MatData data;
                Serialization::Deserializer::DeserializeAny(data, entry.path().string());

                // Create a new MaterialData instance and populate it from metadata
                auto material = MaterialData::Create(ShaderLibrary::Get("PBR"));/* Pass any required shader or default values */
                //material->SetGUID(metadata.guid);
                material->SetAlbedoColor(data.albedoColor);
                material->SetMetalness(data.metalness);
                material->SetRoughness(data.roughness);
                material->SetAO(data.ao);
                material->SetEmission(data.emission);
                material->SetTransparency(data.transparency);
                material->SetTiling(data.tiling);
                material->SetOffset(data.offset);

                // Add the material to the MaterialTable (if it doesn't already exist)
                //AddMaterialByGUID(data.guid);
            }
        }
    }

    void MaterialTable::ClearMaterials(){
        if (mMaterials.size() > 1) {
            mMaterials.erase(mMaterials.begin() + 1, mMaterials.end());
        }
    }
}
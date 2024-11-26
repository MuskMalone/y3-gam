#include "pch.h"
#include "MaterialTable.h"
#include "MaterialData.h"
#include "Asset/Assetables/Material/MaterialAsset.h"
#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"
#include <Graphics/Renderer.h>
#include "Asset/AssetManager.h"

namespace Graphics {
    std::unordered_map<IGE::Assets::GUID, uint32_t> MaterialTable::mGUIDToIndexMap;
    std::vector<std::shared_ptr<MaterialData>> MaterialTable::mMaterials;
    GLuint MaterialTable::mMaterialSSBO;
    std::vector<MaterialProperties> MaterialTable::mMaterialPropsBuffer;


    bool MaterialTable::UpdateMaterialPropsBuffer(){
        bool anyUpdated{ false };
        for (size_t i = 0; i < mMaterials.size(); ++i) {
            if (mMaterials[i] && mMaterials[i]->IsModified()) {
                MaterialProperties& props = mMaterialPropsBuffer[i];
                props.Tiling = mMaterials[i]->GetTiling();
                props.Offset = mMaterials[i]->GetOffset();
                props.AlbedoColor = glm::vec4(mMaterials[i]->GetAlbedoColor(),1.f);
                props.Metalness = mMaterials[i]->GetMetalness();
                props.Roughness = mMaterials[i]->GetRoughness();
                props.Transparency = mMaterials[i]->GetTransparency();
                props.AO = mMaterials[i]->GetAO();

                mMaterials[i]->ClearModifiedFlag();
                anyUpdated = true;
            }
        }

        return anyUpdated;
    }

    void MaterialTable::UploadMaterialProps(){
        GLCALL(glNamedBufferSubData(
            mMaterialSSBO,
            0, // Offset
            sizeof(MaterialProperties) * mMaterialPropsBuffer.size(),
            mMaterialPropsBuffer.data() // Pointer to the CPU-side buffer
        ));
    }

    void MaterialTable::Init(uint32_t maxMaterials) {

        mMaterialPropsBuffer.resize(maxMaterials); // Reserve CPU-side buffer space

        // Generate and bind the SSBO
        GLCALL(glCreateBuffers(1, &mMaterialSSBO));
        // Allocate GPU memory for the SSBO
        glNamedBufferStorage(
            mMaterialSSBO,
            sizeof(MaterialProperties) * maxMaterials,
            nullptr, // No initial data
            GL_DYNAMIC_STORAGE_BIT // Allow dynamic updates to the buffer
        );

         // Bind the SSBO to binding point 0
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mMaterialSSBO);
    }
    void MaterialTable::Shutdown() {
        if (mMaterialSSBO) {
            glDeleteBuffers(1, &mMaterialSSBO); // Delete the buffer
            mMaterialSSBO = 0;
        }
        mMaterialPropsBuffer.clear(); // Clear the CPU-side buffer
    }
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
        IGE_ASSETMGR.LoadRef<IGE::Assets::MaterialAsset>(guid);
       auto & materialData = IGE_ASSETMGR.GetAsset<IGE::Assets::MaterialAsset>(guid)->mMaterial;
        //std::shared_ptr<MaterialData> materialData{};
        if (!materialData) {
            // Handle missing material (use a default material if needed)
            std::stringstream ss;
            ss << "Material with GUID: " << guid << " not found.\n";
            Debug::DebugLogger::GetInstance().LogError(ss.str());
            materialData = MaterialData::Create("PBR", "null"); //TODO CHNAGE THIS
        }

        // Add the new material to the table
        uint32_t newIndex = static_cast<uint32_t>(mMaterials.size());
        mMaterials.push_back(materialData);
        mGUIDToIndexMap[guid] = newIndex;

        UpdateMaterialPropsBuffer();
        UploadMaterialProps();

        return newIndex;
    }

    void MaterialTable::DeleteMaterial(IGE::Assets::GUID const& guid) {
        // Check if the material exists in the map
        auto it = mGUIDToIndexMap.find(guid);
        if (it == mGUIDToIndexMap.end()) {
            Debug::DebugLogger::GetInstance().LogError("DeleteMaterial: Material with specified GUID not found.");
            return;  // Material does not exist
        }

        uint32_t indexToDelete = it->second;

        // Remove from mMaterials vector
        mMaterials.erase(mMaterials.begin() + indexToDelete);

        // Remove the GUID from mGUIDToIndexMap
        mGUIDToIndexMap.erase(it);

        // Update indices in mGUIDToIndexMap for materials shifted down
        for (auto& [key, index] : mGUIDToIndexMap) {
            if (index > indexToDelete) {
                index--;  // Decrement indices after the removed material
            }
        }
    }

    uint32_t MaterialTable::GetMaterialIndexByGUID(const IGE::Assets::GUID& guid) {
      auto it = mGUIDToIndexMap.find(guid);
      if (it != mGUIDToIndexMap.end()) {
        return it->second;
      }
      return static_cast<uint32_t>(0);
    }

    std::shared_ptr<MaterialData> const& MaterialTable::GetMaterial(uint32_t index = 0) {
        return mMaterials[index];
    }

    std::shared_ptr<MaterialData> const& MaterialTable::GetMaterialByGUID(const IGE::Assets::GUID& guid){
        auto it = mGUIDToIndexMap.find(guid);
        if (it != mGUIDToIndexMap.end()) {
            return GetMaterial(it->second);
        }

#ifdef _DEBUG
        std::cerr << "Warning: Material with GUID " << " not found in MaterialTable.\n"; //@TODO CHANGE THIS TO DEBUGLOGGER
#endif
        throw Debug::Exception<MaterialTable>(Debug::LVL_ERROR, Msg("Material With GUID not found in MaterialTable"));
        return nullptr;
    }

    void MaterialTable::ApplyMaterialTextures(std::shared_ptr<Graphics::Shader> const& shader, unsigned batchStart, unsigned batchEnd) {
      IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
      unsigned const matCount{ batchEnd - batchStart + 1 };

      am.GetAsset<IGE::Assets::TextureAsset>(Renderer::GetWhiteTexture())->mTexture.Bind(Texture::sDefaultAlbedoUnit);
      am.GetAsset<IGE::Assets::TextureAsset>(Renderer::GetWhiteTexture())->mTexture.Bind(Texture::sDefaultNormalUnit); // change to normal Tex

      // Initialize texture unit arrays to default values
      std::vector<int> albedoTextureUnits(sMaterialsPerBatch + 1, Texture::sDefaultAlbedoUnit);
      std::vector<int> normalTextureUnits(sMaterialsPerBatch + 1, Texture::sDefaultNormalUnit);

      for (unsigned matIdx{ batchStart + 1 }, i{ 1 }; i <= matCount; ++i, ++matIdx) {
        std::shared_ptr<MaterialData> const& material = mMaterials[matIdx];

        IGE::Assets::GUID const albedoMap{ material->GetAlbedoMap() },
          normalMap{ material->GetNormalMap() };

        // Only bind the albedo map if it�s unique (not the default texture)
        if (albedoMap != Renderer::GetWhiteTexture()) {
          am.LoadRef<IGE::Assets::TextureAsset>(albedoMap);
          am.GetAsset<IGE::Assets::TextureAsset>(albedoMap)->mTexture.Bind(i);
          albedoTextureUnits[i] = static_cast<int>(i);  // Assign this unique texture unit to the shader array
        }

        // Only bind the normal map if it�s unique (not the default texture)
        if (normalMap != Renderer::GetWhiteTexture()) { // @TODO Change to normal Tex
          am.LoadRef<IGE::Assets::TextureAsset>(normalMap);
          am.GetAsset<IGE::Assets::TextureAsset>(normalMap)->mTexture.Bind(Texture::sDefaultNormalUnit + i);
          normalTextureUnits[i] = static_cast<int>(Texture::sDefaultNormalUnit + i);
        }
      }

      // Set texture unit arrays in the shader; any unused slots will point to default textures
      shader->SetUniform("u_AlbedoMaps", albedoTextureUnits.data(), static_cast<unsigned>(albedoTextureUnits.size()));
      shader->SetUniform("u_NormalMaps", normalTextureUnits.data(), static_cast<unsigned>(normalTextureUnits.size()));
    }

    // Bind textures for all materials to the shader

    void MaterialTable::ApplyMaterialTextures(std::shared_ptr<Shader> const& shader) {
        size_t size = mMaterials.size();
        if (size <= 1) return;  // Exit if there's only the default material

        IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
        // Bind the default textures only once @TODO CHANGE THIS TO SOMEWHERE ELSE
        am.GetAsset<IGE::Assets::TextureAsset>(Renderer::GetWhiteTexture())->mTexture.Bind(Texture::sDefaultAlbedoUnit);
        am.GetAsset<IGE::Assets::TextureAsset>(Renderer::GetWhiteTexture())->mTexture.Bind(Texture::sDefaultNormalUnit); // change to normal Tex

        // Initialize texture unit arrays to default values
        std::vector<int> albedoTextureUnits(sMaterialsPerBatch + 1, Texture::sDefaultAlbedoUnit);
        std::vector<int> normalTextureUnits(sMaterialsPerBatch + 1, Texture::sDefaultNormalUnit);

        // Start from index 1 to skip the default material
        for (uint32_t i = 1; i < mMaterials.size() && i < 16; ++i) {  // Up to 16 unique textures
            std::shared_ptr<MaterialData> const& material = mMaterials[i];

            // Get the material�s textures
            auto albedoMap = material->GetAlbedoMap();
            auto normalMap = material->GetNormalMap();

            // Only bind the albedo map if it�s unique (not the default texture)
            if (albedoMap != Renderer::GetWhiteTexture()) {
              am.LoadRef<IGE::Assets::TextureAsset>(albedoMap);
              am.GetAsset<IGE::Assets::TextureAsset>(albedoMap)->mTexture.Bind(i);
              albedoTextureUnits[i] = static_cast<int>(i);  // Assign this unique texture unit to the shader array
            }

            // Only bind the normal map if it�s unique (not the default texture)
            if (normalMap != Renderer::GetWhiteTexture()) { // @TODO Change to normal Tex
              am.LoadRef<IGE::Assets::TextureAsset>(normalMap);
              am.GetAsset<IGE::Assets::TextureAsset>(normalMap)->mTexture.Bind(Texture::sDefaultNormalUnit + i);
              normalTextureUnits[i] = static_cast<int>(Texture::sDefaultNormalUnit + i);
            }
        }

        // Set texture unit arrays in the shader; any unused slots will point to default textures
        shader->SetUniform("u_AlbedoMaps", albedoTextureUnits.data(), static_cast<unsigned>(albedoTextureUnits.size()));
        shader->SetUniform("u_NormalMaps", normalTextureUnits.data(), static_cast<unsigned>(normalTextureUnits.size()));
    }

    IGE::Assets::GUID MaterialTable::CreateAndImportMatFile(const std::string& name){
        // Step 1: Create a new material with default properties
        auto newMaterial = MaterialData::Create("PBR", "new");
        newMaterial->SetName(name);
        newMaterial->SetShaderName("PBR"); // Set a default shader if applicable
        newMaterial->SetAlbedoColor({ 1.0f, 1.0f, 1.0f });
        newMaterial->SetMetalness(0.5f);
        newMaterial->SetRoughness(0.5f);
        // Set other default properties as needed...

        // Step 2: Add the new material to the MaterialTable
        //mMaterials.push_back(newMaterial);

        // Step 3: Generate a unique filename based on the material's name
        std::stringstream ss;
        ss << gMaterialDirectory << newMaterial->GetName() << ".mat"; // You can use a unique identifier if needed
        std::string filename = ss.str();

        // Step 5: Serialize the material data to the file
        Serialization::Serializer::SerializeAny(*newMaterial, filename);

        ss << "New material created and saved at " << filename << std::endl;
        Debug::DebugLogger::GetInstance().LogInfo(ss.str());

        IGE::Assets::GUID guid = IGE_ASSETMGR.ImportAsset<IGE::Assets::MaterialAsset>(filename);

        AddMaterialByGUID(guid);
        return guid;

    }

    void MaterialTable::SaveMaterials() {
        for (size_t i = 1; i < mMaterials.size(); ++i) {  // Start from index 1 to skip the default material at index 0
            auto& material = mMaterials[i];
            if (!material) continue;

            // Create a unique filename for each material based on its GUID
            std::stringstream ss;
            ss << gMaterialDirectory << material->GetName() << i << ".mat";
           // std::string filename = "Materials/" + material->GetGUID().ToString() + ".mat";
            std::string filename = ss.str();

            // Serialize the metadata into the file
            Serialization::Serializer::SerializeAny(*material, filename);
        }
    }

    void MaterialTable::SaveMaterial(IGE::Assets::GUID const& guid) {
      IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
      std::string const output{ am.GUIDToPath(guid) };

      // if material not in table, serialize from asset manager
      if (!mGUIDToIndexMap.contains(guid)) {
        am.LoadRef<IGE::Assets::MaterialAsset>(guid);
        Serialization::Serializer::SerializeAny(*am.GetAsset<IGE::Assets::MaterialAsset>(guid)->mMaterial, output);
        return;
      }

      // else retrieve it
      Serialization::Serializer::SerializeAny(*mMaterials[mGUIDToIndexMap.at(guid)], output);
    }

    std::shared_ptr<MaterialData> MaterialTable::LoadMaterial(std::string const& fp) {
        // Ensure the file exists
        if (!std::filesystem::exists(fp)) {
            std::stringstream ss;
            ss << "Failed to load material data, file " << fp << " does not exist.\n";
            Debug::DebugLogger::GetInstance().LogError(ss.str());
            return nullptr;
        }

        std::shared_ptr<MaterialData> material{ std::make_shared<MaterialData>() };
        // Deserialize the material data from the specified file
        Serialization::Deserializer::DeserializeAny(*material, fp);

        // Retrieve the shader name from `data` (if it's stored there) or use a default
        std::string const shaderName = material->GetShaderName().empty() ? "PBR" : material->GetShaderName();
        
        material->SetShader(shaderName);

        // Return the loaded material
        return material;
    }

    void MaterialTable::ClearMaterials(){
        if (mMaterials.size() > 1) {
            mMaterials.erase(mMaterials.begin() + 1, mMaterials.end());
        }
    }
}
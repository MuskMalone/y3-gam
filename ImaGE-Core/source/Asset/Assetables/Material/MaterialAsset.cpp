#include "pch.h"
#include "MaterialAsset.h"

IGE::Assets::MaterialAsset::MaterialAsset(std::string const& fp): mMaterial{*Graphics::MaterialTable::LoadMaterial(fp)} {

}

IGE::Assets::GUID IGE::Assets::MaterialAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps& metadata){
    // Check if the file path is valid
    if (!IsValidFilePath(fp)) {
        return GUID{}; // Return null GUID if invalid
    }

    // Define the new file path within the assets directory
    std::filesystem::path path{ fp };
    std::string fileName = GetFileName(fp);
    std::string fileExt = GetFileExtension(fp);
    std::string assetFilePath = cMaterialDirectory + fileName + fileExt;

    // Create directories if they do not exist
    CreateDirectoryIfNotExists(cMaterialDirectory);

    // Copy the file to the assets folder
    if (!CopyFileToAssets(fp, assetFilePath)) {
        return GUID{}; // Return null GUID if the copy fails
    }

    // Populate metadata
    metadata.emplace("path", assetFilePath);
    newFp = assetFilePath;

    return GUID{ GUID::Seed{} }; // Generate a unique GUID
}

void* IGE::Assets::MaterialAsset::Load(GUID guid){
    // Convert GUID to file path
    std::string fp = AssetManager::GetInstance().GUIDToPath(guid);

    // Check if the file path is valid, otherwise create a default MaterialAsset
    if (!IsValidFilePath(fp)) {
        std::cerr << "Invalid file path for GUID: " << guid << "\n";
        return new MaterialAsset(Graphics::MaterialData::CreateDefault());
    }

    std::filesystem::path const path{ fp };
    std::string filename = GetFileName(fp);
    std::string fileext = GetFileExtension(fp);

    // Ensure the material directory exists for compiled materials
    CreateDirectoryIfNotExists(cMaterialDirectory + cCompiledDirectory);

    // Define the final file path in the compiled directory
    std::string const finalfp = cMaterialDirectory + cCompiledDirectory + filename + ".mat";

    // Validate the file extension (assuming ".mat" for materials)
    if (fileext != ".mat") {
        throw Debug::Exception<MaterialAsset>(Debug::LVL_ERROR, Msg("Unsupported file extension: " + fileext));
    }

    // Load material data from the file path using MaterialTable
    auto materialData = Graphics::MaterialTable::LoadMaterial(finalfp);
    if (!materialData) {
        std::cerr << "Failed to load material data from file: " << finalfp << "\n";
        return nullptr;
    }

    // Return a new MaterialAsset with the loaded material data
    return new MaterialAsset(*materialData);
}

void IGE::Assets::MaterialAsset::Unload(MaterialAsset* ptr, GUID guid){
	delete ptr;
}

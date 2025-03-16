#include <pch.h>
#include "ModelAsset.h"
#include <filesystem>
#include <string>
#include <Graphics/Mesh/IMSH.h>
#include "Graphics/Mesh/MeshFactory.h"
#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"

namespace {
	bool ReadBoolean(IGE::Assets::AssetMetadata::AssetProps const& metadata, const char* key);
}

namespace IGE {
	namespace Assets {
		ModelAsset::ModelAsset(std::string const& fp) : mMeshSource{ 
			(IsValidFilePath(fp)) ? 
				Graphics::MeshFactory::CreateModelFromImport(fp) :
				Graphics::MeshFactory::CreateModelFromString(fp)
		}
		{
		}
		IGE::Assets::GUID ModelAsset::Import(
			std::string const& fp, 
			std::string& newFp, 
			AssetMetadata::AssetProps& metadata)
		{
			//check if file is valid
			if (!IsValidFilePath(fp)) {
				return GUID{ }; // return a null guid if invalid
			}

			//new file path for the asset to live in
			std::filesystem::path const path{ fp };
			std::string const filename { GetFileName(fp) };
			std::string const fileext { GetFileExtension(fp) };
			std::string const inputModelPath{ cModelDirectory + filename + fileext };

			CreateDirectoryIfNotExists(cModelDirectory);

			//copy the file over to the assets folder
			if (!CopyFileToAssets(fp, inputModelPath)) {
				return GUID{}; // invalid guid
			}

			std::string const compiledDir{ cModelDirectory + cCompiledDirectory };
			CreateDirectoryIfNotExists(compiledDir);

			Debug::DebugLogger::GetInstance().LogInfo("Model detected. Converting to .imsh file...");

			Graphics::AssetIO::ImportSettings settings{};
			// same keys as in AssetBrowser.cpp
			settings.staticMesh = ReadBoolean(metadata, IMSH_IMPORT_STATIC);
			settings.flipUVs = ReadBoolean(metadata, IMSH_IMPORT_FLIP_UV);
			settings.recenterMesh = ReadBoolean(metadata, IMSH_IMPORT_RECENTER);
			settings.normalizeScale = ReadBoolean(metadata, IMSH_IMPORT_NORM_SCALE);
			settings.minimalFlags = ReadBoolean(metadata, IMSH_IMPORT_MIN_FLAGS);

			Graphics::AssetIO::IMSH imsh{ fp, settings };
			imsh.WriteToBinFile(compiledDir + filename + gMeshFileExt);
			Debug::DebugLogger::GetInstance().LogInfo(("Added " + filename + fileext) + " to assets");

			//populate metadata (right now its only path)
			metadata.metadata["path"] = inputModelPath;
			newFp = inputModelPath;
			return GUID{ GUID::Seed{} }; // generate a unique guid
		}
		void* ModelAsset::Load(GUID guid)
		{
			//converting in imsh is done here
			//assuming that this is an imsh file that is added via the assets folder
			std::string fp{ AssetManager::GetInstance().GUIDToPath(guid) };
			if (!IsValidFilePath(fp)) {
				return new ModelAsset(fp);
			}
			std::filesystem::path const path{ fp };
			std::string filename{ GetFileName(fp) };
			std::string fileext{ GetFileExtension(fp) };
			CreateDirectoryIfNotExists(cModelDirectory + cCompiledDirectory);
			std::string const finalfp{ cModelDirectory + cCompiledDirectory + GetFileName(fp) + gMeshFileExt };

			//copy the file to a "Compiled" folder
			//if (fileext == ".imsh") { // if it is already a compiled format
			//	//return new ModelAsset(fp);
			//	finalfp = cModelDirectory + cCompiledDirectory + filename + fileext;
			//	CopyFileToAssets(fp, finalfp);
			//}

			if (std::string(gSupportedModelFormats).find(fileext) == std::string::npos) {
				throw Debug::Exception<ModelAsset>(Debug::LVL_ERROR, Msg("couldnt compile" + fileext + "to imsh"));
			}
			
			return new ModelAsset(
				finalfp
			);
		}
		void ModelAsset::Unload(ModelAsset* ptr, GUID guid)
		{
			delete ptr;
		}
	}	// namespace Assets
}	// namespace IGE

namespace {
	bool ReadBoolean(IGE::Assets::AssetMetadata::AssetProps const& metadata, const char* key) {
		auto const result{ metadata.metadata.find(key) };
		return result != metadata.metadata.cend() && result->second == "1";
	}
}

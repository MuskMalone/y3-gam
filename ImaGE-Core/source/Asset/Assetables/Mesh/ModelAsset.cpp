#include <pch.h>
#include "ModelAsset.h"
#include <filesystem>
#include <string>
#include <Graphics/AssetIO/IMSH.h>
#include "Graphics/MeshFactory.h"
#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"
namespace IGE {
	namespace Assets {
		ModelAsset::ModelAsset(std::string const& fp) : mMeshSource{ 
			(!IsValidFilePath(fp)) ? 
				Graphics::MeshFactory::CreateModelFromString(fp) : 
				Graphics::MeshFactory::CreateModelFromImport(fp)
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
			std::string filename { GetFileName(fp) };
			std::string fileext { GetFileExtension(fp) };
			std::string inputModelPath{ cModelDirectory + filename + fileext };
			CreateDirectoryIfNotExists(cModelDirectory);


			//copy the file over to the assets folder
			if (!CopyFileToAssets(fp, inputModelPath)) {
				return GUID{}; // invalid guid
			}

			//assuming that this is an imsh file that is added via the assets folder
			// tch: i commented this out cuz this if clause should be for loading
			//if (IsDirectoriesEqual(inputModelPath, fp) && fileext == ".imsh") {
			//	return AssetManager::GetInstance().PathToGUID(inputModelPath);
			//}
			//if (std::string(gSupportedModelFormats).find(path.extension().string()) != std::string::npos) {
			//	Graphics::AssetIO::IMSH imsh{ fp };
			//	Debug::DebugLogger::GetInstance().LogInfo("Model detected. Converting to .imsh file...");
			//	imsh.WriteToBinFile(path.stem().string());
			//	Debug::DebugLogger::GetInstance().LogInfo(("Added " + path.stem().string() + gMeshFileExt) + " to assets");
			//	newFp = cModelDirectory + GetFileName(fp) + gMeshFileExt;
			//	return GUID{ GUID::Seed{} };
			//}

			//populate metadata (right now its only path)
			metadata.emplace("path", inputModelPath);
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
			std::string filename { GetFileName(fp) };
			std::string fileext { GetFileExtension(fp) };
			CreateDirectoryIfNotExists(cModelDirectory + cCompiledDirectory);

			std::string finalfp{};
			//copy the file to a "Compiled" folder
			if (fileext == ".imsh") { // if it is already a compiled format
				//return new ModelAsset(fp);
				finalfp = cModelDirectory + cCompiledDirectory + filename + fileext;
				CopyFileToAssets(fp, finalfp);
			}
			else if (std::string(gSupportedModelFormats).find(fileext) != std::string::npos) {
				finalfp = cModelDirectory + cCompiledDirectory + GetFileName(fp) + gMeshFileExt;

				// CHENG EN: Added this check for now cause i need import to NOT run if its alr been converted
				if (!std::filesystem::exists(finalfp)) {
					Graphics::AssetIO::IMSH imsh{ fp };
					Debug::DebugLogger::GetInstance().LogInfo("Model detected. Converting to .imsh file...");
					imsh.WriteToBinFile(path.stem().string());
					Debug::DebugLogger::GetInstance().LogInfo(("Added " + filename + fileext) + " to assets");
				}
			}
			else {
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
	}
}
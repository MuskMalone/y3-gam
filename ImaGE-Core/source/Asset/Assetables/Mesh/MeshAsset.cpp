#include <pch.h>
#include "MeshAsset.h"
#include <filesystem>
#include <string>
#include <Graphics/AssetIO/IMSH.h>
#include "Graphics/MeshFactory.h"
#include "Asset/AssetUtils.h"
namespace IGE {
	namespace Assets {
		MeshAsset::MeshAsset(std::string const& fp) : mMeshSource{ 
			(!IsValidFilePath(fp)) ? 
				Graphics::MeshFactory::CreateModelFromString(fp) : 
				Graphics::MeshFactory::CreateModelFromImport(fp)
		}
		{
		}

		IGE::Assets::GUID MeshAsset::Import(std::string const& fp)
		{
			if (!IsValidFilePath(fp)) {
				return GUID{ fp };
			}
			std::filesystem::path const path{ fp };
			std::string const filename{ GetFileName(fp) };
			std::string const fileext{ GetFileExtension(fp) };
			std::string const inputModelPath{ cModelDirectory + filename + fileext };
			CreateDirectoryIfNotExists(cModelDirectory);
			//assuming that this is an imsh file that is added via the assets folder
			if (IsDirectoriesEqual(inputModelPath, fp) && fileext == ".imsh")
				return GUID{ GetAbsolutePath(inputModelPath) }; 
			if (std::string(gSupportedModelFormats).find(path.extension().string()) != std::string::npos) {
				
				Graphics::AssetIO::IMSH imsh{ fp };
				Debug::DebugLogger::GetInstance().LogInfo("Model detected. Converting to .imsh file...");
				imsh.WriteToBinFile(path.stem().string());
				Debug::DebugLogger::GetInstance().LogInfo(("Added " + path.stem().string() + gMeshFileExt) + " to assets");
				return GUID{ GetAbsolutePath(cModelDirectory + GetFileName(fp) + gMeshFileExt) };
			}
			return GUID{};
		}

		void* MeshAsset::Load(GUID guid) {
			return new MeshAsset(guid.GetSeed());
		}

		void MeshAsset::Unload(MeshAsset* ptr, GUID guid) {
			delete ptr;
		}
	}
}
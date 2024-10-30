#include <pch.h>
#include "AudioAsset.h"
#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"
#include "Asset/Assetables/AssetExtensions.h"
namespace IGE {
	namespace Assets {
		IGE::Assets::GUID AudioAsset::Import(
			std::string const& fp, 
			std::string & newFp, 
			AssetMetadata::AssetProps& metadata)
		{
            // Copy the image file to the assets folder
            std::string filename { GetFileName(fp) };
            std::string fileext { GetFileExtension(fp) };
			std::string inputAudioPath{ cAudioDirectory + filename + fileext };//GetAbsolutePath(cAudioDirectory + filename + fileext) };
            CreateDirectoryIfNotExists(cAudioDirectory);

            if (!CopyFileToAssets(fp, inputAudioPath)) {
				return GUID{};
            }

			//populate the metatata//populate metadata (right now its only path)
			newFp = inputAudioPath;
			metadata.emplace("path", newFp);
			return GUID{ GUID::Seed{} };
		}
		void* AudioAsset::Load(GUID guid)
		{
			//im not sure what to compile for audio assets
			std::string fp{ AssetManager::GetInstance().GUIDToPath(guid) };
			std::filesystem::path const path{ fp };
			std::string filename { GetFileName(fp) };
			std::string fileext { GetFileExtension(fp) };
			CreateDirectoryIfNotExists(cAudioDirectory + cCompiledDirectory);
			//copy to Compiled folder
			if (cAudioExtensions.find(fileext) == cAudioExtensions.end())
				throw Debug::Exception<AudioAsset>(Debug::LVL_ERROR, Msg("unsupported file type " + fileext));
			std::string finalfp{ cAudioDirectory + cCompiledDirectory + filename + fileext };
			CopyFileToAssets(fp, finalfp);

			return reinterpret_cast<void*>(new AudioAsset{ finalfp });
		}
		void AudioAsset::Unload(AudioAsset* ptr, GUID guid)
		{
			delete ptr;
		}
	}
}
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
			std::string& newFp,
			AssetMetadata::AssetProps& metadata)
		{
			// Copy the image file to the assets folder
			std::string const filename{ GetFileName(fp) };
			std::string const fileext{ GetFileExtension(fp) };
			std::string const inputAudioPath{ cAudioDirectory + filename + fileext };//GetAbsolutePath(cAudioDirectory + filename + fileext) };
			CreateDirectoryIfNotExists(cAudioDirectory);
			CreateDirectoryIfNotExists(cAudioDirectory + cCompiledDirectory);

			if (!CopyFileToAssets(fp, inputAudioPath)) {
				return GUID{};
			}

			//copy to Compiled folder
			if (cAudioExtensions.find(fileext) == cAudioExtensions.end())
				throw Debug::Exception<AudioAsset>(Debug::LVL_ERROR, Msg("unsupported file type " + fileext));

			std::string const finalfp{ cAudioDirectory + cCompiledDirectory + filename + fileext };
			CopyFileToAssets(fp, finalfp);

			//populate the metatata//populate metadata (right now its only path)
			newFp = inputAudioPath;
			metadata.metadata["path"] = newFp;
			return GUID{ GUID::Seed{} };
		}
		void* AudioAsset::Load(GUID guid) {
			//im not sure what to compile for audio assets
			std::string const& fp{ AssetManager::GetInstance().GUIDToPath(guid) };
			std::string const finalfp{ cAudioDirectory + cCompiledDirectory + GetFileNameWithExtension(fp) };

			return reinterpret_cast<void*>(new AudioAsset{ finalfp });
		}
		void AudioAsset::Unload(AudioAsset* ptr, GUID guid)
		{
			delete ptr;
		}
	}	// namespace Assets
}	// namespace IGE

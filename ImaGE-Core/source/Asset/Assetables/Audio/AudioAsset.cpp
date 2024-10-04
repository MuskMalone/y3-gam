#include <pch.h>
#include "AudioAsset.h"
#include "Asset/AssetUtils.h"
namespace IGE {
	namespace Assets {
		IGE::Assets::GUID AudioAsset::Import(std::string const& fp)
		{
            // Copy the image file to the assets folder
            std::string filename { GetFileName(fp) };
            std::string fileext { GetFileExtension(fp) };
            std::string inputAudioPath{ GetAbsolutePath(cAudioDirectory + filename + fileext) };
            CreateDirectoryIfNotExists(cAudioDirectory);
            if (IsDirectoriesEqual(inputAudioPath, fp)) return GUID{ GetAbsolutePath(inputAudioPath) };
            if (!CopyFileToAssets(fp, inputAudioPath)) {
                return -1;
            }
            return GUID{ GetAbsolutePath(inputAudioPath) };
		}
		void* AudioAsset::Load(GUID guid)
		{
			return reinterpret_cast<void*>(new AudioAsset{guid.GetSeed()});
		}
		void AudioAsset::Unload(AudioAsset* ptr, GUID guid)
		{
			delete ptr;
		}
	}
}
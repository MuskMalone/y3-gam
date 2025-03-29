#pragma once
#include "../../SmartPointer.h"
#include "Asset/AssetMetadata.h"
#include "Audio/AudioManager.h"

namespace IGE::Assets {
	struct VideoAsset : public RefCounted {
		std::string mVideoPath;

		VideoAsset(std::string const& compiledPath) : mVideoPath{ compiledPath } {};

		static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps&);
		static void* Load(GUID guid);
		static void Unload(VideoAsset* ptr, GUID guid);
	};
}

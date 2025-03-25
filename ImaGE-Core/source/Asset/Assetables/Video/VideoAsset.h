#pragma once
#include "../../SmartPointer.h"
#include "Graphics/Texture.h"
#include "Asset/AssetMetadata.h"

namespace IGE::Assets {
	struct VideoAsset : public RefCounted {
		Graphics::Texture mTexture;

		VideoAsset(std::string const& fp) : mTexture{ fp, false } {};

		static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps&);
		static void* Load(GUID guid);
		static void Unload(VideoAsset* ptr, GUID guid);
	};
}

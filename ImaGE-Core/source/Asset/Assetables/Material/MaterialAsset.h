#pragma once
#include "Asset/AssetMetadata.h"
namespace IGE {

	namespace Assets {

		const std::string cMaterialDirectory{ std::string(gAssetsDirectory) + "Materials\\" };
		struct Asset : public RefCounted {
			Systems::Font mFont;
			FontAsset(std::string const& fp) : mFont{ fp } {};
			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
				AssetMetadata::AssetProps& metadata);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] FontAsset* ptr, GUID guid);
		};

	}

}
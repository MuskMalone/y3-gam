#pragma once
#include "Asset/SmartPointer.h"
#include "Asset/AssetMetadata.h"

namespace IGE {
	namespace Assets {
		const std::string cFontDirectory{ std::string(gAssetsDirectory) + "Fonts\\" };

		struct FontAsset : public RefCounted {
			FontAsset(std::string const& fp);

			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
				AssetMetadata::AssetProps&);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] FontAsset* ptr, GUID guid);
		};
	}
}
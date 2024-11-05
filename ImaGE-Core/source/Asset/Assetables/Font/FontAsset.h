#pragma once
#include "Asset/SmartPointer.h"
#include "Asset/AssetMetadata.h"
#include "Core/Systems/TextSystem/TextSystem.h"

namespace IGE {

	namespace Assets {

		const std::string cFontDirectory{ std::string(gAssetsDirectory) + "Fonts\\" };
		struct FontAsset : public RefCounted {
			Systems::Font mFont;
			FontAsset(std::string const& fp) : mFont{ fp } {};
			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
				AssetMetadata::AssetProps& metadata);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] FontAsset* ptr, GUID guid);
		};

	}

}
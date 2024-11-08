#pragma once
#include "Asset/AssetMetadata.h"
#include "Graphics/MaterialData.h"

namespace IGE {

	namespace Assets {

		const std::string cMaterialDirectory{ std::string(gAssetsDirectory) + "Materials\\" };
		struct MaterialAsset : public RefCounted {
			Graphics::MaterialData mMaterial;
			MaterialAsset(std::string const& fp) /*: mMaterial{ fp } {}*/;
			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
				AssetMetadata::AssetProps&);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] MaterialAsset* ptr, GUID guid);
		};

	}

}
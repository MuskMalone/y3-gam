#pragma once
#include "Asset/SmartPointer.h"
#include "Graphics/Mesh/MeshFactory.h"
#include "Asset/AssetMetadata.h"
namespace IGE {
	namespace Assets {
		const std::string cModelDirectory{ std::string(gAssetsDirectory) + "Models\\" };
		struct ModelAsset : public RefCounted {
			Graphics::MeshSource mMeshSource;
			ModelAsset(std::string const& fp);
			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
				AssetMetadata::AssetProps&);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] ModelAsset* ptr, GUID guid);
		};
	}
}
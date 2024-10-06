#pragma once
#include "Asset/SmartPointer.h"
#include "Graphics/MeshFactory.h"
namespace IGE {
	namespace Assets {
		const std::string cModelDirectory{ std::string(gAssetsDirectory) + "Models\\" };
		struct MeshAsset : public RefCounted {
			Graphics::MeshSource mMeshSource;
			MeshAsset(std::string const& fp);
			static IGE::Assets::GUID Import(std::string const& fp);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] MeshAsset* ptr, GUID guid);
		};
	}
}
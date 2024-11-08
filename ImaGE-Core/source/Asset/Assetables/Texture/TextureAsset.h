#pragma once
#include "../../SmartPointer.h"
#include "Graphics/Texture.h"
#include "DirectXTex.h"
#include "Asset/AssetMetadata.h"
namespace IGE {
	namespace Assets {
		const std::string cTextureDirectory{ std::string(gAssetsDirectory) + "Textures\\" };
		struct TextureAsset : public RefCounted {
			Graphics::Texture mTexture;
			TextureAsset(std::string const& fp) : mTexture{ fp, false } {};
			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
				AssetMetadata::AssetProps&);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] TextureAsset* ptr, GUID guid);
		};

	}
}

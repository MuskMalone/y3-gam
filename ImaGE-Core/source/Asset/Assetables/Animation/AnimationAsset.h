#pragma once
#include "../../SmartPointer.h"
#include "Asset/AssetMetadata.h"
#include <Animation/AnimationData.h>

namespace IGE {
	namespace Assets {
		struct AnimationAsset : public RefCounted {
			AnimationAsset(std::string const& fp);

			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps&);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] AnimationAsset* ptr, GUID guid);

			Anim::AnimationData mAnimData;
		};
	}
}
#include <pch.h>
#include "AnimationAsset.h"
#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"
#include "Asset/Assetables/AssetExtensions.h"
#include <Serialization/Deserializer.h>

namespace IGE {
	namespace Assets {
		AnimationAsset::AnimationAsset(std::string const& fp) {
			// deserialize from fp into mAnimData
			mAnimData = Serialization::Deserializer::DeserializeAnimationData(fp);
		}

		IGE::Assets::GUID AnimationAsset::Import(std::string const& fp, std::string& newFp,
			AssetMetadata::AssetProps& metadata) {
			if (!IsValidFilePath(fp)) {
				return GUID{}; // return a null guid if invalid
			}

			CreateDirectoryIfNotExists(gAnimationsDirectory);

			newFp = gAnimationsDirectory + GetFileNameWithExtension(fp);
			metadata.metadata["path"] = newFp;
			return GUID{ GUID::Seed{} };
		}

		void* AnimationAsset::Load(GUID guid) {
			return new AnimationAsset(
				AssetManager::GetInstance().GUIDToPath(guid)
			);
		}

		void AnimationAsset::Unload(AnimationAsset* ptr, GUID guid) { delete ptr; }
	}
}

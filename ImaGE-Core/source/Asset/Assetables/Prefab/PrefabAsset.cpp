#include <pch.h>
#include "PrefabAsset.h"
#include <Asset/AssetUtils.h>
#include <Serialization/Deserializer.h>
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"

namespace IGE {
	namespace Assets {
		PrefabAsset::PrefabAsset(std::string const& fp) :
			mPrefabData{ Serialization::Deserializer::DeserializePrefabToVariant(fp) } {}

		GUID PrefabAsset::Import(std::string const& fp, std::string& newFp, 
			AssetMetadata::AssetProps& metadata) {
			if (!IsValidFilePath(fp)) {
				return GUID{ }; // return a null guid if invalid
			}

			CreateDirectoryIfNotExists(cPrefabsDirectory);

			newFp = cPrefabsDirectory + GetFileNameWithExtension(fp);
			metadata.metadata["path"] = newFp;
			return GUID{ GUID::Seed{} };
		}

		void* PrefabAsset::Load(GUID guid) {
			return new PrefabAsset(
				AssetManager::GetInstance().GUIDToPath(guid)
			);
		}

		void PrefabAsset::Unload(PrefabAsset* ptr, GUID guid) {
			delete ptr;
		}
	}
}
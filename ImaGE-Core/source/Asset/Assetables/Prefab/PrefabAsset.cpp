#include <pch.h>
#include "PrefabAsset.h"
#include <Asset/AssetUtils.h>
#include <Serialization/Deserializer.h>

namespace IGE {
	namespace Assets {
		PrefabAsset::PrefabAsset(std::string const& fp) :
			mPrefabData{ Serialization::Deserializer::DeserializePrefabToVariant(fp) } {}

		GUID PrefabAsset::Import(std::string const& fp) {
			return GUID{ GetAbsolutePath(cPrefabsDirectory + GetFileNameWithExtension(fp)) };
		}

		void* PrefabAsset::Load(GUID guid) {
			return new PrefabAsset(guid.GetSeed());
		}

		void PrefabAsset::Unload(PrefabAsset* ptr, GUID guid) {
			delete ptr;
		}
	}
}
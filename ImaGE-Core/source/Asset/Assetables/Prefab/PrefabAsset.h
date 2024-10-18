#pragma once
#include <Prefabs/Prefab.h>

namespace IGE {
	namespace Assets {
		const std::string cPrefabsDirectory{ std::string(gPrefabsDirectory) };

		struct PrefabAsset : public RefCounted {
			Prefabs::Prefab mPrefabData;

			PrefabAsset(std::string const& fp);

			static GUID Import(std::string const& fp);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] PrefabAsset* ptr, GUID guid);
		};
	}
}

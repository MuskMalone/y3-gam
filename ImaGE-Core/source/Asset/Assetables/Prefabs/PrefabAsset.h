#pragma once
#include <Asset/SmartPointer.h>
#include <Prefabs/Prefab.h>

namespace IGE {
	namespace Assets {
		const std::string cModelDirectory{ std::string(gPrefabsDirectory) };

		struct PrefabAsset : public RefCounted {
			Prefabs::Prefab mPrefabData;

			PrefabAsset(std::string const& fp);

			static IGE::Assets::GUID Import(std::string const& fp);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] PrefabAsset* ptr, GUID guid);
		};
	}
}

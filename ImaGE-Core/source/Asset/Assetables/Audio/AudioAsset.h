#pragma once
#include "../../SmartPointer.h"
#include "Audio/AudioManager.h"
namespace IGE {
	namespace Assets {
		const std::string cAudioDirectory{ std::string(gAssetsDirectory) + "Audio\\" };
		struct AudioAsset : public RefCounted {
			IGE::Audio::Sound mSound;
			AudioAsset(std::string const& fp) : mSound{ fp } {};
			static IGE::Assets::GUID Import(std::string const& fp);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] AudioAsset* ptr, GUID guid);

		};
	}
}
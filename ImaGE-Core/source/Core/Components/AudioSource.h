#pragma once
#include "Asset/Assetables/Assetables.h"
#include "Audio/AudioManager.h"
namespace Component{
	struct AudioSource {
		
		struct AudioInstance {
			IGE::Assets::GUID guid{};
			IGE::Audio::SoundInvokeSetting playSettings;
			AudioInstance() = default;
		};
		std::unordered_map<std::string, AudioInstance> sounds{};
		
		//honestly i couldve gone with categorizing the sounds instead of one channel group per entity
		//but this way seems less convoluted
		AudioSource();
		void Clear() noexcept;
		void PlaySound(std::string const&) const;
		void RemoveSound(std::string const&);
		void CreateSound(std::string const&);
		void RenameSound(std::string const& currentName, std::string const& newName);
	
		uint64_t channelGroup;

	};
}
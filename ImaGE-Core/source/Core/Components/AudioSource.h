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
		// made it ordered so its always serialized the same
		std::map<std::string, AudioInstance> sounds{};
		
		//honestly i couldve gone with categorizing the sounds instead of one channel group per entity
		//but this way seems less convoluted
		AudioSource();
		void Clear() noexcept;
		void SetSoundPitch(std::string const&, float);
		void SetSoundVolume(std::string const&, float);
		void EnablePostProcessing(std::string const& id, IGE::Audio::SoundInvokeSetting::PostProcessingType type, float param);
		void DisablePostProcessing(std::string const& id);
		void PlaySound(std::string const&) const;
		void PauseSound(std::string const& id) const;
		void StopSound(std::string const& id) const;
		uint32_t GetPlaybackTime(std::string const& id) const;
		void SetPlaybackTime(std::string const& id, uint32_t time);
		void RemoveSound(std::string const&);
		void CreateSound(std::string const&);
		void RenameSound(std::string const& currentName, std::string const& newName);
	
		uint64_t channelGroup;

	};
}
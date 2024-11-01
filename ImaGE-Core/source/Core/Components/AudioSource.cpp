#include <pch.h>
#include "Core/Components/AudioSource.h"
namespace Component {
	AudioSource::AudioSource() : channelGroup{ IGE::Audio::AudioManager::GetInstance().CreateGroup()}
	{

	}
	void AudioSource::Clear() noexcept
	{
		sounds.clear();
	}
	void AudioSource::PlaySound(std::string const& id) const
	{
		//play sound
		auto const& sound{ sounds.at(id) };
		IGE::Audio::AudioManager::GetInstance().PlaySound(sound.guid, sound.playSettings, channelGroup);
	}
	void AudioSource::RemoveSound(std::string const& id)
	{
		sounds.erase(id);
	}
	void AudioSource::CreateSound(IGE::Assets::GUID const& guid)
	{
		//add a new sound according to guid
		sounds.emplace("Sound " + std::to_string(sounds.size()), guid);
	}
	void AudioSource::RenameSound(std::string const& currentName, std::string const& newName) {
		if (sounds.find(newName) != sounds.end()) { // if the new name is already in the map
			Debug::DebugLogger::GetInstance().LogWarning("sound already exists inside AudioSource component!");
			return;
		}
		//if sounds exists
		if (sounds.find(currentName) != sounds.end()) {
			AudioInstance temp{ std::move(sounds.at(currentName)) };
			sounds.erase(currentName);
			sounds.emplace(newName, std::move(temp));
		}
		else {
			Debug::DebugLogger::GetInstance().LogWarning("sound currently does not exist in AudioSource component");
		}
	}
}
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
		if (sounds.find(id) != sounds.end()) {
			auto const& sound{ sounds.at(id) };
			IGE::Audio::AudioManager::GetInstance().PlaySound(sound.guid, sound.playSettings, channelGroup);
		}
	}
	void AudioSource::PauseSound(std::string const& id) const {
		if (sounds.find(id) != sounds.end()) {
			auto const& sound{ sounds.at(id) };
			IGE::Audio::AudioManager::GetInstance().PauseSound(sound.guid, sound.playSettings);
		}
	}
	void AudioSource::StopSound(std::string const& id) const {
		if (sounds.find(id) != sounds.end()) {
			auto const& sound{ sounds.at(id) };
			IGE::Audio::AudioManager::GetInstance().StopSound(sound.guid, sound.playSettings);
		}
	}
	void AudioSource::RemoveSound(std::string const& id)
	{
		//stop all the channels first
		auto& instance{sounds.at(id)};
		for (auto channel : instance.playSettings.channels) {
			IGE::Audio::AudioManager::GetInstance().StopChannel(channel);
		}
		sounds.erase(id);
	}
	void AudioSource::CreateSound(std::string const& fp)
	{
		//add a new sound according to guid
		try {
			auto guid{ IGE::Assets::AssetManager::GetInstance().PathToGUID(fp) };
			IGE::Assets::AssetManager::GetInstance().LoadRef<IGE::Assets::AudioAsset>(guid);
			AudioInstance ai{};
			ai.guid = guid;
			sounds.emplace(fp, ai);
		}
		catch ([[maybe_unused]] Debug::Exception<IGE::Assets::AssetManager> const& e) {
			Debug::DebugLogger::GetInstance().LogWarning("couldnt load asset");
		}
		catch (...) {
			Debug::DebugLogger::GetInstance().LogWarning("error while adding sound");
		}
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
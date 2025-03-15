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
	void AudioSource::SetSoundPitch(std::string const& id, float pitch)
	{
		if (sounds.find(id) != sounds.end()) {
			auto& sound{ sounds.at(id) };
			sound.playSettings.pitch = pitch;
		}
	}
	void AudioSource::SetSoundVolume(std::string const& id, float volume)
	{
		glm::clamp(volume, 0.f, 1.f);
		if (sounds.find(id) != sounds.end()) {
			auto & sound{ sounds.at(id) };
			sound.playSettings.volume = volume;
		}
	}
	void AudioSource::EnablePostProcessing(std::string const& id, IGE::Audio::SoundInvokeSetting::PostProcessingType type, float param)
	{
		//play sound
		if (sounds.find(id) != sounds.end()) {
			auto & sound{ sounds.at(id) };
			sound.playSettings.enablePostProcessing = true;
			sound.playSettings.processingType = type;
			sound.playSettings.postProcessingParameter = param;
		}
	}
	void AudioSource::DisablePostProcessing(std::string const& id)
	{
		//play sound
		if (sounds.find(id) != sounds.end()) {
			auto& sound{ sounds.at(id) };
			sound.playSettings.enablePostProcessing = false;
		}
	}
	void AudioSource::PlaySound(std::string const& id) const
	{
		//play sound
		if (sounds.find(id) != sounds.end()) {
			auto const& sound{ sounds.at(id) };
			sound.playSettings.name = id;
			IGE::Audio::AudioManager::GetInstance().PlaySound(sound.guid, sound.playSettings, channelGroup, id);
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
	void AudioSource::StopAllSounds() const
	{
		for (auto const& [name, sound] : sounds) {
			IGE::Audio::AudioManager::GetInstance().StopSound(sound.guid, sound.playSettings);
		}
	}
	//gets the playback time of the most recent channel in milliseconds
	uint32_t AudioSource::GetPlaybackTime(std::string const& id) const {
		if (sounds.find(id) != sounds.end()) {
			auto& sound{ sounds.at(id) };
			auto channel{ *sound.playSettings.channels.begin() };
			uint32_t playbackTime{};
			channel->getPosition(&playbackTime, FMOD_TIMEUNIT_MS);
			return playbackTime;
		}
		return 0;
	}
	//sets the playback time of the most recent channel in milliseconds
	void AudioSource::SetPlaybackTime(std::string const& id, uint32_t time) {
		if (sounds.find(id) != sounds.end()) {
			auto& sound{ sounds.at(id) };
			for (auto& channel : sound.playSettings.channels) {
				channel->setPosition(time, FMOD_TIMEUNIT_MS);
			}
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
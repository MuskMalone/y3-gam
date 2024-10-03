#pragma once
#include <fmod.hpp>
#include <unordered_map>
#include <string>

//1: No filtering
//0: Full filtering, completely muffles
//0.5: Medium filtering
const float LOWPASS_FILTER_VALUE{ 0.5f };

class AudioManager {
public:
	bool Initialize(); //Creates FMOD system
	void Update(); //Updates FMOD, to be called every frame
	void Release(); //DELETES THE AUDIO SYSTEM, ONLY CALL AT END OF PROGRAM
	//CHANNEL GROUP FUNCTIONS TO MANAGE MULTIPLE SOUNDS AT ONCE
	//Creates a group of sound with input name, create separate groups for different categories, e.g., BGM, SFX, and VOC.
	FMOD::ChannelGroup* CreateGroup(const char* name);
	FMOD::Sound* Sound;
	//Sets overall volume of the entire group
	void SetGroupVolume(const char* name, float volume);
	//Get the volume of a specific group., float* is the input volume
	float GetGroupVolume(const char* name);
	//Sets the low pass filter of the group, to create a 'muffled'sound (eg. for underwater/ behind wall)
	void SetGroupFilter(const char* name, float filter);
	//Stops all sounds in a specific group
	void StopGroup(const char* name);
	//Resumes the sound in a specific group
	void ResumeGroup(const char* name);
	//Pauses the sound in a specific group
	void PauseGroup(const char* name);
	//Returns true if the group sound is paused, to prevent resume of playing groups
	bool IsGroupPaused(const char* name);
	//Sets the BGM of the scene
	void SetBGM(const char* name);
	//Restarts scene BGM from beginning, eg.use after a cutscene/battle
	void RestartBGM();
	//Sets the ambience of the scene, background sound
	void SetAmbience(const char* name);

	//SOUND MANAGEMENT FUNCTIONS
	//Releases all sounds from memory(audio manager), eg.moved to a new scene, we remove all old sounds thats no longer needed
	void ReleaseAllSounds();
	//Add a sound to FMOD and audio manager
	FMOD::Sound* AddSound(const char* path, const char* name);
	//Add music to FMOD and audio manager
	FMOD::Sound* AddMusic(const char* path, const char* name);
	//Add ambience to FMOD and audio manager
	FMOD::Sound* AddAmbience(const char* path, const char* name);
	//Plays loaded sound, FMOD_VECTOR* -> FMOD uses this vector to position the sound in 3D space
	//Each sound's position will be set relative to the listener's position
	//FMOD::Channel* PlaySound(const char* sound, const char* channelGroup = nullptr, int loops = 0, FMOD_VECTOR* position = nullptr);
	FMOD::Channel* PlaySound(const char* sound, const char* channelGroup = nullptr, int loops = 0, FMOD_VECTOR* position = nullptr, float minDistance = std::numeric_limits<float>::min(), float maxDistance = std::numeric_limits<float>::max());
	//Free a speciifc sound from FMOD and audio manager, free up memory when a sound is no longer needed
	void FreeSound(const char* sound);

	//Gets the FMOD system instance
	FMOD::System* GetSystem();

	//UTILITY AND GETTER FUNCTIONS
	//Get names of all sounds currently loaded in the audio manager
	std::vector<std::string> GetSoundNames();
	//Get paths of all sounds in sound folder
	std::vector<std::string> GetSoundPaths();
	//Get paths of all music in music folder (background music)
	std::vector<std::string> GetMusicPaths();
	//Get paths of all ambience in ambience folder
	std::vector<std::string> GetAmbiencePaths();
	//Get name of the current BGM
	std::string GetCurrentBGM();
	//Get name of the current ambience
	std::string GetCurrentAmbience();
	//Scans the sound, music, and ambience folders for available files
	//Updates the list of audio files available for loading
	void UpdateAudioDirectory();
	//Use in game loop, function sets the player’s position, velocity, and orientation in 3D space and updates the FMOD system’s listener attributes 
	void SetPlayerAttributes(const FMOD_3D_ATTRIBUTES& attributes);

private:
	//for managing FMOD system
	FMOD::System* system{};
	//stores the sounds by name. FMOD provides a handle to each sound through FMOD::Sound
	std::unordered_map<std::string, FMOD::Sound*> data{};
	//for managing groups of audio channels by grouping different sound effects, ambient sounds, etc., 
	std::unordered_map<std::string, FMOD::ChannelGroup*> group{};
	std::unordered_map<std::string, std::list<FMOD::Channel*>> channels{};
	std::string currentBGM{};
	std::string originalBGM{};
	std::string currentAmbience{};
	std::vector<std::string> soundPaths{};
	std::vector<std::string> musicPaths{};
	std::vector<std::string> ambiencePaths{};
};

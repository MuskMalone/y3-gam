#pragma once
#include <fmod.hpp>
#include <unordered_map>
#include <string>
#include <Core/GUID.h>
//1: No filtering
//0: Full filtering, completely muffles
//0.5: Medium filtering
const float LOWPASS_FILTER_VALUE{ 0.5f };
namespace IGE {
	namespace Audio {
		struct Sound {
			const std::string mKey;
			const uint32_t mKeyhash;
			Sound(std::string const& fp);
			~Sound();
			void PlaySound();
		};
		class AudioManager {
		public:

			static bool Initialize(); //Creates FMOD system
			static void Update(); //Updates FMOD, to be called every frame
			static void Release(); //DELETES THE AUDIO SYSTEM, ONLY CALL AT END OF PROGRAM
			//CHANNEL GROUP FUNCTIONS TO MANAGE MULTIPLE SOUNDS AT ONCE
			//Creates a group of sound with input name, create separate groups for different categories, e.g., BGM, SFX, and VOC.
			static FMOD::ChannelGroup* CreateGroup(const char* name);
			//Sets overall volume of the entire group
			static void SetGroupVolume(const char* name, float volume);
			//Get the volume of a specific group., float* is the input volume
			static float GetGroupVolume(const char* name);
			//Sets the low pass filter of the group, to create a 'muffled'sound (eg. for underwater/ behind wall)
			static void SetGroupFilter(const char* name, float filter);
			//Stops all sounds in a specific group
			static void StopGroup(const char* name);
			//Resumes the sound in a specific group
			static void ResumeGroup(const char* name);
			//Pauses the sound in a specific group
			static void PauseGroup(const char* name);
			//Returns true if the group sound is paused, to prevent resume of playing groups
			static bool IsGroupPaused(const char* name);
			//Sets the BGM of the scene
			static void SetBGM(const char* name);
			//Restarts scene BGM from beginning, eg.use after a cutscene/battle
			static void RestartBGM();
			//Sets the ambience of the scene, background sound
			static void SetAmbience(const char* name);

			//SOUND MANAGEMENT FUNCTIONS
			//Releases all sounds from memory(audio manager), eg.moved to a new scene, we remove all old sounds thats no longer needed
			static void ReleaseAllSounds();
			//Add a sound to FMOD and audio manager
			static FMOD::Sound* AddSound(const char* path, uint32_t name);
			//Add music to FMOD and audio manager
			static FMOD::Sound* AddMusic(const char* path, uint32_t name);
			//Add ambience to FMOD and audio manager
			static FMOD::Sound* AddAmbience(const char* path, uint32_t name);
			//Plays loaded sound, FMOD_VECTOR* -> FMOD uses this vector to position the sound in 3D space
			//Each sound's position will be set relative to the listener's position
			//FMOD::Channel* PlaySound(const char* sound, const char* channelGroup = nullptr, int loops = 0, FMOD_VECTOR* position = nullptr);
			static FMOD::Channel* PlaySound(uint32_t sound, const char* channelGroup = nullptr, int loops = 0, FMOD_VECTOR* position = nullptr, float minDistance = std::numeric_limits<float>::min(), float maxDistance = std::numeric_limits<float>::max());
			//Free a speciifc sound from FMOD and audio manager, free up memory when a sound is no longer needed
			static void FreeSound(uint32_t sound);

			//Gets the FMOD system instance
			static FMOD::System* GetSystem();

			//UTILITY AND GETTER FUNCTIONS
			//Get names of all sounds currently loaded in the audio manager
			static std::vector<std::string> GetSoundNames();
			//Get paths of all sounds in sound folder
			static std::vector<std::string> GetSoundPaths();
			//Get paths of all music in music folder (background music)
			static std::vector<std::string> GetMusicPaths();
			//Get paths of all ambience in ambience folder
			static std::vector<std::string> GetAmbiencePaths();
			//Get name of the current BGM
			static std::string GetCurrentBGM();
			//Get name of the current ambience
			static std::string GetCurrentAmbience();
			//Scans the sound, music, and ambience folders for available files
			//Updates the list of audio files available for loading
			static void UpdateAudioDirectory();
			//Use in game loop, function sets the player’s position, velocity, and orientation in 3D space and updates the FMOD system’s listener attributes 
			static void SetPlayerAttributes(const FMOD_3D_ATTRIBUTES& attributes);

		private:
			//for managing FMOD system
			static FMOD::System* _mSystem;
			//stores the sounds by name. FMOD provides a handle to each sound through FMOD::Sound
			static std::unordered_map<uint32_t, FMOD::Sound*> _mData;
			//for managing groups of audio channels by grouping different sound effects, ambient sounds, etc., 
			static std::unordered_map<std::string, FMOD::ChannelGroup*> _mGroup;
			static std::unordered_map<std::string, std::list<FMOD::Channel*>> _mChannels;
			static std::string _mCurrentBGM;
			static std::string _mOriginalBGM;
			static std::string _mCurrentAmbience;
			static std::vector<std::string> _mSoundPaths;
			static std::vector<std::string> _mMusicPaths;
			static std::vector<std::string> _mAmbiencePaths;
		};
	}
}

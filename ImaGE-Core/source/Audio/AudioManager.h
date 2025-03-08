#pragma once
#include <fmod.hpp>
#include <unordered_map>
#include <string>
#include <Core/GUID.h>
#include <Asset/AssetManager.h>
#include "Singleton/ThreadSafeSingleton.h"
#include "Events/EventCallback.h"
#include <Audio/AudioSystem.h>
//1: No filtering
//0: Full filtering, completely muffles
//0.5: Medium filtering
const float LOWPASS_FILTER_VALUE{ 0.5f };
namespace IGE {
	namespace Audio {
		struct SoundInvokeSetting {
			enum class RolloffType {
				LINEAR,
				LOGARITHMIC, 
				NONE
			};
			enum class PostProcessingType {
				REVERB, 
				ECHO, 
				DISTORTION,
				CHORUS,
			};
			// Position in 3D space for 3D sounds
			glm::vec3 position{0.0f, 0.0f, 0.0f};

			// Basic sound settings
			float volume{ 1.0f };        // Volume level (0.0 to 1.0)
			float pitch{ 1.0f };         // Pitch multiplier (1.0 = normal pitch)
			float pan{ 0.0f };           // Stereo panning (-1.0 = left, 1.0 = right)
			bool mute{ false };          // Mute/unmute sound
			bool loop{ false };          // Enable/disable looping
			bool playOnAwake{ true };    // Play sound immediately on creation

			// 3D sound settings
			float dopplerLevel{ 1.0f };  // Doppler effect intensity
			float minDistance{ 1.0f };   // Minimum distance for 3D attenuation
			float maxDistance{ 500.0f }; // Maximum distance for 3D attenuation
			// Custom rolloff curves for advanced sound properties
			RolloffType rolloffType{ RolloffType::NONE };

			bool enablePostProcessing{ false };
			PostProcessingType processingType{PostProcessingType::REVERB};
			float postProcessingParameter{1500.f};

			//atm, its used to watch whether the channel has stopped playing, 
			//then set the channel pointer back to nullptr
			static FMOD_RESULT FMODChannelCallback(FMOD_CHANNELCONTROL* chanCtrl, FMOD_CHANNELCONTROL_TYPE type,
				FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commanddata1, void* commanddata2);
			mutable std::unordered_set<FMOD::Channel*> channels; // not for imgui
			mutable bool paused{ false }; // not fo rimgui
			mutable bool manualStop{ false }; // not for imgui
		};

		struct Sound {

			const std::string mKey;
			const uint32_t mKeyhash;
			~Sound();
			Sound(std::string const& fp);
			void PlaySound(SoundInvokeSetting const&, FMOD::ChannelGroup* group);
		};

		class AudioManager : public ThreadSafeSingleton<AudioManager> {
		public:
			AudioManager();
			~AudioManager();
			 bool Initialize(); //Creates FMOD system
			 //void Update(); //Updates FMOD, to be called every frame
			 void Release(); //DELETES THE AUDIO SYSTEM, ONLY CALL AT END OF PROGRAM
			//CHANNEL GROUP FUNCTIONS TO MANAGE MULTIPLE SOUNDS AT ONCE
			//Creates a nameless group of sound
			 uint64_t CreateGroup();
			//Sets overall volume of the entire group
			 void SetGroupVolume(std::string const& name, float volume);
			//Get the volume of a specific group., float* is the input volume
			 float GetGroupVolume(std::string const& name);
			//Sets the low pass filter of the group, to create a 'muffled'sound (eg. for underwater/ behind wall)
			 void SetGroupFilter(std::string const& name, float filter);
			//Stops all sounds in a specific group
			 void StopGroup(std::string const& name);
			//Resumes the sound in a specific group
			 void ResumeGroup(std::string const& name);
			//Pauses the sound in a specific group
			 void PauseGroup(std::string const& name);
			//Returns true if the group sound is paused, to prevent resume of playing groups
			 bool IsGroupPaused(std::string const& name);

			 void StopChannel(FMOD::Channel* channel);
			//SOUND MANAGEMENT FUNCTIONS
			//Releases all sounds from memory(audio manager), eg.moved to a new scene, we remove all old sounds thats no longer needed
			 void ReleaseAllSounds();
			//Add a sound to FMOD and audio manager
			 FMOD::Sound* AddSound(std::string const& path, uint32_t name);

			 void PlaySound(uint32_t sound, SoundInvokeSetting const& settings, FMOD::ChannelGroup* group);

			//Free a speciifc sound from FMOD and audio manager, free up memory when a sound is no longer needed
			 void PlaySound(IGE::Assets::GUID const& guid, SoundInvokeSetting const&, uint64_t group);
			 void PauseSound(IGE::Assets::GUID const& guid, SoundInvokeSetting const&);
			 void StopSound(IGE::Assets::GUID const& guid, SoundInvokeSetting const&);
			 void FreeSound(uint32_t sound);

			//Gets the FMOD system instance
			 FMOD::System* GetSystem();

			////UTILITY AND GETTER FUNCTIONS
			////Get names of all sounds currently loaded in the audio manager
			// std::vector<std::string> GetSoundNames();
			////Get paths of all sounds in sound folder
			// std::vector<std::string> GetSoundPaths();
			////Scans the sound, music, and ambience folders for available files
			////Updates the list of audio files available for loading
			// void UpdateAudioDirectory();
			//Use in game loop, function sets the player’s position, velocity, and orientation in 3D space and updates the FMOD system’s listener attributes 
			// void SetPlayerAttributes(const FMOD_3D_ATTRIBUTES& attributes);

		private:
			friend AudioSystem;
			friend SoundInvokeSetting;
			using ChannelGroupGUID = Core::GUID<FMOD::Channel>;
			//for managing FMOD system
			 FMOD::System* mSystem;
			//stores the sounds by name. FMOD provides a handle to each sound through FMOD::Sound
			 std::unordered_map<uint32_t, FMOD::Sound*> mData;
			//for managing groups of audio channels by grouping different sound effects, ambient sounds, etc., 
			 std::unordered_map<ChannelGroupGUID, FMOD::ChannelGroup*> mGroup;
			// std::unordered_map<std::string, std::list<FMOD::Channel*>> _mChannels;
			 EVENT_CALLBACK_DECL(HandleSystemEvents);
			 bool mSceneStarted{false};
			 bool mSceneStopped{true}; // scene starts from a stopped state
			 bool mScenePaused{false};
		};

	}
}

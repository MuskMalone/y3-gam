#include <pch.h>
#include "AudioManager.h"
#include <iostream>
#include <filesystem>
#include <fmod_errors.h>
#include <Asset/IGEAssets.h>
#include <Core/Components/Components.h>
#include <Events/EventManager.h>
#include "Scenes/SceneManager.h"

//#define AUDIO_VERBOSE

namespace IGE {
    namespace Audio {
        AudioManager::AudioManager()
        {
            SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &AudioManager::HandleSystemEvents, this);
            Initialize();
        }
        AudioManager::~AudioManager()
        {
            std::cerr << "audio destructed>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
            Release();
        }
        //std::unordered_map<std::string, std::list<FMOD::Channel*>> AudioManager::_mChannels;
        bool AudioManager::Initialize()
        {
            FMOD_RESULT result;
            mSystem = nullptr;

            //creating the FMOD system object, inorder to use FMOD's functionalities
            result = FMOD::System_Create(&mSystem);
            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                return false;
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Creation", true);
            }

            //initializing FMOD with 3D audio support
            //FMOD_INIT_3D_RIGHTHANDED flag tells FMOD that the 3D coordinate system in the engine is right-handed -> means using 3D audio
            result = mSystem->init(1024, FMOD_INIT_3D_RIGHTHANDED, 0);

            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                return false;
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Creation", true);
            }

            // Set up listener attributes (THE PLAYER)
            FMOD_VECTOR listenerPosition = { 0.0f, 0.0f, 0.0f };    // Initial position (x, y, z)
            FMOD_VECTOR listenerVelocity = { 0.0f, 0.0f, 0.0f };    // Initial velocity
            FMOD_VECTOR forwardDirection = { 0.0f, 0.0f, 1.0f };    // Forward vector (direction listener is facing)
            FMOD_VECTOR upDirection = { 0.0f, 1.0f, 0.0f };         // Up vector

            //remove for 2d audio
            //result = mSystem->set3DListenerAttributes(0, &listenerPosition, &listenerVelocity, &forwardDirection, &upDirection);
            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Could not set listener attributes: " + str, true);
                return false;
            }
            //remove for 2d audio
            //mSystem->set3DSettings(1.0f, 1.0f, 1.0f); // Distance factor, rolloff scale, Doppler scale
            //fetching the master channel group, to control the volume and other properties of all sounds globally
            
            //_mSystem->getMasterChannelGroup(&_mGroup["Master"]);

            return true;
        }
        void AudioManager::Release()
        {
            FMOD_RESULT result;
            //clear the entire FMOD system when the game shuts down
            ReleaseAllSounds();
            result = mSystem->release();

            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                //Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
            }
            else
            {
                //Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Shutdown", true);
            }
        }

        uint64_t AudioManager::CreateGroup()
        {
            //checks if a channel group with the given name already exists in the group map
            //count() return 1 if the group exist

            //create a guid based on the name
            ChannelGroupGUID groupguid{ ChannelGroupGUID::Seed{} };
            //when group not exist
            FMOD::ChannelGroup* channelgroup;
            FMOD_RESULT result;
            result = mSystem->createChannelGroup(nullptr, &channelgroup);//creates a new channel group with that name

            mGroup.emplace(groupguid, channelgroup);//stores the new channel group in the group map, using channelgroupguid as the key

            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
            }
#ifdef AUDIO_VERBOSE
            else
            {
                Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD Channel Group Creation", true);
            }
#endif
            return groupguid;//return the channel group
        }

        void AudioManager::SetGroupVolume(std::string const& name, float volume)
        {
            //checks if the given group(by name) exists in the group map
            if (mGroup.find(name) != mGroup.end())
            {
                FMOD_RESULT result;
                result = mGroup[name]->setVolume(volume);//if it exists, sets the volume of the group
                if (result != FMOD_OK)
                {
                    std::string str(FMOD_ErrorString(result));
                    Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                }
            }
#ifdef AUDIO_VERBOSE
            else
            {
                Debug::DebugLogger::GetInstance().LogInfo("Set group volume to " + std::to_string(volume), true);
            }
#endif
        }

        float AudioManager::GetGroupVolume(std::string const& name)
        {
            float volume = 0.0f;
            //checks if the given group(by name) exists in the group map
            if (mGroup.find(name) != mGroup.end())
            {
                FMOD_RESULT result;
                //if exist, it gets the the current volume of the channel group and store in volume
                result = mGroup[name]->getVolume(&volume);
                if (result != FMOD_OK)
                {
                    std::string str(FMOD_ErrorString(result));
                    Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                }
#ifdef AUDIO_VERBOSE
                else
                {
                    Debug::DebugLogger::GetInstance().LogInfo("Group Volume is" + std::to_string(volume), true);
                }
#endif
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
            }
            return volume;
        }

        void AudioManager::StopGroup(std::string const& name)
        {
            if (mGroup.find(name) != mGroup.end())
            {
                FMOD_RESULT result;
                result = mGroup[name]->stop();
                if (result != FMOD_OK)
                {
                    std::string str(FMOD_ErrorString(result));
                    Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                }
#ifdef AUDIO_VERBOSE
                else
                {
                    Debug::DebugLogger::GetInstance().LogInfo("Stop Group", true);
                }
#endif
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
            }
        }

        void AudioManager::ResumeGroup(std::string const& name)
        {
            if (mGroup.find(name) != mGroup.end())
            {
                FMOD_RESULT result;
                result = mGroup[name]->setPaused(false);
                if (result != FMOD_OK)
                {
                    std::string str(FMOD_ErrorString(result));
                    Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                }
#ifdef AUDIO_VERBOSE
                else
                {
                    Debug::DebugLogger::GetInstance().LogInfo("Resume Group", true);
                }
#endif
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
            }
        }

        void AudioManager::PauseGroup(std::string const& name)
        {
            if (mGroup.find(name) != mGroup.end())
            {
                FMOD_RESULT result;
                result = mGroup[name]->setPaused(true);
                if (result != FMOD_OK)
                {
                    std::string str(FMOD_ErrorString(result));
                    Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                }
#ifdef AUDIO_VERBOSE
                else
                {
                    Debug::DebugLogger::GetInstance().LogInfo("Pause Group", true);
                }
#endif
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
            }
        }

        bool AudioManager::IsGroupPaused(std::string const& name)
        {
            bool status = false;
            //checks if the given group(by name) exists in the group map
            if (mGroup.find(name) != mGroup.end())
            {
                FMOD_RESULT result;
                //if exist, get the pause state and store in status
                result = mGroup[name]->getPaused(&status);
                if (result != FMOD_OK)
                {
                    std::string str(FMOD_ErrorString(result));
                    Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                    return false;
                }
            }
            else
            {
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Group not found: " + std::string(name), true);
                return false;
            }
            return status;  // return the paused status (true if paused, false otherwise)
        }

        void AudioManager::StopChannel(FMOD::Channel* channel)
        {
            bool isplaying{ false };
            FMOD_RESULT result{ channel->isPlaying(&isplaying) };
            if (result == FMOD_OK) {
                channel->stop();
            }
        }

        //Add a sound to FMOD and audio manager
        FMOD::Sound* AudioManager::AddSound(std::string const& filepath, uint32_t namehash)
        {
            //checks if the sound already exist
            if (mData.find(namehash) != mData.end() && mData[namehash] != nullptr)
            {
                return mData[namehash];
            }

            //remove for 2d audio
            FMOD_RESULT result = mSystem->createSound(filepath.c_str(), FMOD_DEFAULT | FMOD_2D, 0, &mData[namehash]); //non-3D sound
            //FMOD_RESULT result = mSystem->createSound(filepath.c_str(), FMOD_3D, 0, &mData[namehash]);//create 3D sound

            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
            }
#ifdef AUDIO_VERBOSE
            else
            {
                Debug::DebugLogger::GetInstance().LogInfo("Successfully added sound: " + std::string(filepath), true);
            }
#endif

            return mData[namehash];
        }

        void AudioManager::PlaySound(uint32_t sound, SoundInvokeSetting const& settings, FMOD::ChannelGroup* group)
        {
            FMOD_RESULT result;

            // Set the loop count for the sound based on the `loop` setting
            result = mData[sound]->setMode(settings.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            result = mData[sound]->setLoopCount(settings.loop ? -1 : 0);
            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                return;
            }

            FMOD::Channel* temp = nullptr;

            // Play the sound without grouping it
            result = mSystem->playSound(mData[sound], group, true, &temp); // Start paused to set initial parameters
            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                return;
            }

            // Set volume, pitch, and pan for the channel
            if (temp)
            {
                temp->setVolume(settings.volume);
                temp->setPitch(settings.pitch);
                temp->setPan(settings.pan);
                temp->setMute(settings.mute);

                // Convert position to FMOD_VECTOR
                FMOD_VECTOR fmodPosition = { settings.position.x, settings.position.y, settings.position.z };
                //remove for 2d audio
                temp->setMode(FMOD_2D);
                ////temp->setMode(FMOD_3D); // Set the channel to 3D mode
                //temp->set3DAttributes(&fmodPosition, nullptr); // Set the 3D position
                //temp->set3DMinMaxDistance(settings.minDistance, settings.maxDistance); // Set min/max distance
                //temp->set3DDopplerLevel(settings.dopplerLevel); // Set Doppler level
                //// Apply rolloff
                //switch (settings.rolloffType) {
                //case SoundInvokeSetting::RolloffType::LINEAR:
                //    temp->setMode(FMOD_3D_LINEARROLLOFF);
                //    break;
                //case SoundInvokeSetting::RolloffType::LOGARITHMIC:
                //    temp->setMode(FMOD_3D_INVERSEROLLOFF);
                //    break;
                //default:
                //    temp->setMode(FMOD_3D | FMOD_3D_INVERSEROLLOFF);
                //    temp->set3DMinMaxDistance(1e6f, 1e6f);  // Large min and max distance
                //    break;
                //}
            }

            // Unpause the channel to start playback
            result = temp->setPaused(false);
            if (result != FMOD_OK)
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
                return;
            }

#ifdef AUDIO_VERBOSE
            Debug::DebugLogger::GetInstance().LogInfo("Playing sound: " + std::to_string(sound), true);
#endif
            temp->setCallback(
                settings.FMODChannelCallback
            );
            temp->setUserData(const_cast<SoundInvokeSetting*>(&settings));
            settings.channels.insert(temp);
        }

        void AudioManager::PlaySound(IGE::Assets::GUID const& guid, SoundInvokeSetting const& settings, uint64_t group)
        {
            if (settings.paused) {
                for (auto& channel : settings.channels) {
                    channel->setPaused(false);
                }
                settings.paused = false;
                return; // dont run rest of function
            }
            //gets the SoundAsset ref
            if (!IGE::Assets::AssetManager::GetInstance().IsGUIDValid<IGE::Assets::AudioAsset>(guid)) {
                IGE_ASSETMGR.LoadRef<IGE::Assets::AudioAsset>(guid);
            }
            if (IGE::Assets::AssetManager::GetInstance().IsGUIDValid<IGE::Assets::AudioAsset>(guid)) {
                auto audioref{ IGE_ASSETMGR.GetAsset<IGE::Assets::AudioAsset>(guid) };
                auto& sound{ audioref->mSound };
                sound.PlaySound(settings, mGroup.at(group));
            }
            else {
                Debug::DebugLogger::GetInstance().LogError("sound " + std::to_string(guid) + " does not exist");
            }
            settings.paused = false;
        }

        void AudioManager::PauseSound(IGE::Assets::GUID const& guid, SoundInvokeSetting const& setting)
        {
            //gets the SoundAsset ref
            if (!setting.paused) {
                for (auto& channel : setting.channels) {
                    channel->setPaused(true);
                }
                setting.paused = true;
            }
        }

        void AudioManager::StopSound(IGE::Assets::GUID const& guid, SoundInvokeSetting const& setting)
        {
            while (setting.channels.size()) {
                (*setting.channels.begin())->stop();
            }
            setting.paused = false;
        }

        void AudioManager::FreeSound(uint32_t sound)
        {
            //freeing a specific sound from memory 
            //1.releasing the sound in FMOD 2. removing it from the data map.
            if (mData.find(sound) != mData.end() && mData[sound] != nullptr)
            {
                mData[sound]->release();
                mData.erase(sound);
            }
            else
            {
                //Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Freeing non-existent sound: " + std::to_string(sound), true);
            }
        }

        void AudioManager::ReleaseAllSounds()
        {
            size_t soundCount = 0;
            //loop releases each sound stored in the data map
            for (auto const& sound : mData)
            {
                //ensure no nullptr sounds are released
                if (sound.second != nullptr)
                {
                    sound.second->release();
                    ++soundCount;
                }
            }
            //after releasing the sounds, the data map is cleared 
            mData.clear();
        }

        FMOD::System* AudioManager::GetSystem()
        {
            return mSystem;
        }

        void AudioManager::SetGroupFilter(std::string const& name, float filter)
        {

        }

        EVENT_CALLBACK_DEF(AudioManager, HandleSystemEvents) {
            auto const& state{ CAST_TO_EVENT(Events::SceneStateChange)->mNewState };
            if (state == Events::SceneStateChange::NewSceneState::STARTED) {
                mSceneStarted = true;
            }
            if (state == Events::SceneStateChange::NewSceneState::STOPPED || state == Events::SceneStateChange::NewSceneState::CHANGED) {
                if (state == Events::SceneStateChange::CHANGED) { // this is for game app
                    mSceneStarted = true;
                }
                mSceneStopped = true;
                //auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioSource>() };
                //for (auto entity : rbsystem) {
                //    auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                //    auto grpiter{ mGroup.find(audiosource.channelGroup) };
                //    if (grpiter != mGroup.end()) {
                //        grpiter->second->stop();
                //    }
                //}
                FMOD::ChannelGroup* mastergrp{ };
                mSystem->getMasterChannelGroup(&mastergrp);
                mastergrp->stop();
            }
            if (state == Events::SceneStateChange::NewSceneState::PAUSED) {
                mScenePaused = true;
            }
        }

        Sound::Sound(std::string const& fp) : mKey{ fp }, mKeyhash{ IGE::Core::Fnv1a32(fp.c_str(), fp.size()) } {
            AudioManager::GetInstance().AddSound(mKey, mKeyhash);
        }
        Sound::~Sound()
        {
            try {
                AudioManager::GetInstance().FreeSound(mKeyhash);
            }
            catch (...) {
              IGE_DBGLOGGER.LogError("[AudioManager] Failed to free sound" + mKey);
            }
        }
        void Sound::PlaySound(SoundInvokeSetting const& settings, FMOD::ChannelGroup* group) {
            AudioManager::GetInstance().PlaySound(mKeyhash, settings, group);
        }
        FMOD_RESULT SoundInvokeSetting::FMODChannelCallback(
            FMOD_CHANNELCONTROL* chanCtrl, FMOD_CHANNELCONTROL_TYPE type, 
            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commanddata1, void* commanddata2) 
        {
            // Retrieve the instance pointer from user data
            FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(chanCtrl);
            int loopCount{ };
            channel->getLoopCount(&loopCount);
            if (loopCount != 0) return FMOD_OK;
            //if (channel->getLoopCount())
            void* userData = nullptr;
            channel->getUserData(&userData);
            
            if (userData && !IGE::Audio::AudioManager::GetInstance().mSceneStopped) {
                try {
                    SoundInvokeSetting* settings = static_cast<SoundInvokeSetting*>(userData);
                    settings->channels.erase(channel); // Remove channel from active list
#ifdef AUDIO_VERBOSE
                    Debug::DebugLogger::GetInstance().LogInfo("sound has finished playing, removing channel ptr");
#endif
                }
                catch (...) {
                    Debug::DebugLogger::GetInstance().LogWarning("audio instance doesnt exist");
                }
            }
            return FMOD_OK;
        }

    }
}
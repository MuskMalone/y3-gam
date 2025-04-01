#include <pch.h>
#include <Audio/AudioManager.h>
#include <Audio/AudioSystem.h>
#include "Scenes/SceneManager.h"
#include <Core/Components/Components.h>
#include <fmod_errors.h>
#include <Asset/AssetManager.h>
namespace IGE {
	namespace Audio {
		AudioSystem::AudioSystem(const char* name) : Systems::System{ name }
		{}
        void AudioSystem::Update()
        {
            auto& mgr{ AudioManager::GetInstance() };

            //track updates to position
            //track whether guids are valid
            //remove any channels that are invalid
            {
                auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                for (auto entity : rbsystem) {
                    auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                    auto& xfm{ rbsystem.get<Component::Transform>(entity) };
                    for (auto& sound : audiosource.sounds) {
                        auto& setting{ sound.second.playSettings };
                        setting.position = xfm.worldPos;
                        // Remove channels that are no longer playing
                        std::erase_if(setting.channels, [](FMOD::Channel* channel) {
                            bool isPlaying = false;
                            if (channel) {
                                channel->isPlaying(&isPlaying);
                            }
                            return !isPlaying;
                            });
                        for (auto channel : sound.second.playSettings.channels) {
                            channel->setPitch(sound.second.playSettings.pitch);
                            if (sound.second.playSettings.mute) {
                                channel->setVolume(0);
                            }
                            else {
                                //std::cout << ECS::Entity{ entity }.GetComponent<Component::Tag>().tag << " setting volume" << std::endl;
                                float volumeModifier{ ((setting.isBGM) ? AudioManager::GetInstance().mBGMVolume : AudioManager::GetInstance().mSFXVolume) * AudioManager::GetInstance().mGlobalVolume };
                                channel->setVolume(sound.second.playSettings.volume * volumeModifier);
                            }

                            FMOD_VECTOR fmodPosition = { setting.position.x, setting.position.y, setting.position.z };
                            channel->set3DAttributes(&fmodPosition, 0);
                        }
                    }
                }
            }{
                auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::Video, Component::Transform>() };
                for (auto entity : rbsystem) {
                    auto& video{ rbsystem.get<Component::Video>(entity) };
                    auto& xfm{ rbsystem.get<Component::Transform>(entity) };
                    if (mgr.mGroup.find(video.channelGroup) != mgr.mGroup.end()) {
                        FMOD::ChannelGroup* videoSoundGrp = mgr.mGroup[video.channelGroup];
                        int numChannels = 0;
                        videoSoundGrp->getNumChannels(&numChannels);
                        video.audioPlaySettings.position = xfm.worldPos;

                        for (int i = 0; i < numChannels; ++i) {
                            FMOD::Channel* channel = nullptr;
                            videoSoundGrp->getChannel(i, &channel);
                            if (channel) {
                                channel->setPitch(video.audioPlaySettings.pitch);
                                if (video.audioPlaySettings.mute) {
                                    channel->setVolume(0);
                                }
                                else {
                                    //std::cout << ECS::Entity{ entity }.GetComponent<Component::Tag>().tag << " setting volume" << std::endl;
                                    float volumeModifier{ ((video.audioPlaySettings.isBGM) ? AudioManager::GetInstance().mBGMVolume : AudioManager::GetInstance().mSFXVolume) * AudioManager::GetInstance().mGlobalVolume };
                                    channel->setVolume(video.audioPlaySettings.volume * volumeModifier);
                                }

                                FMOD_VECTOR fmodPosition = { video.audioPlaySettings.position.x, video.audioPlaySettings.position.y, video.audioPlaySettings.position.z };
                                channel->set3DAttributes(&fmodPosition, 0);

                            }
                        }
                    }
                }
            }
            {//only accounts for one audio listener
                auto listenersystem{ mEntityManager.GetAllEntitiesWithComponents<Component::Transform, Component::Camera>() };
                //gets the main camera
                for (auto entity : listenersystem){
                    ECS::Entity e{ entity };
                    if (e.GetTag() == "MainCamera") {
                        auto& xfm{ e.GetComponent<Component::Transform>() };
                        auto& camera{ e.GetComponent<Component::Camera>() };
                        auto forwardVec{ camera.GetForwardVector() };
                        auto upVec{ camera.GetUpVector() };
                        // Listener setup
                        FMOD_VECTOR listenerPos = { xfm.worldPos.x, xfm.worldPos.y, xfm.worldPos.z };
                        FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
                        FMOD_VECTOR listenerForward = { forwardVec.x, forwardVec.y, forwardVec.z };
                        FMOD_VECTOR listenerUp = { upVec.x, upVec.y, upVec.z };
                        mgr.mSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
                        break;
                    }
                }

            }
            //auto rbsystem2{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioListener, Component::Transform>() };
            //mgr.mSystem->set3DNumListeners(rbsystem2.)
            //for (auto entity : rbsystem2) {
            //    
            //}
            if (Scenes::SceneManager::GetInstance().GetSceneState() == Scenes::SceneState::PLAYING) {
                {
                    auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                    for (auto entity : rbsystem) {
                        auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                        if (mgr.mSceneStarted && mgr.mScenePaused) {
                            mgr.mGroup[audiosource.channelGroup]->setPaused(false);
                        }
                        for (auto& sound : audiosource.sounds) {
                            auto& setting{ sound.second.playSettings };
                            if (mgr.mSceneStarted && mgr.mSceneStopped && setting.playOnAwake) {
                                audiosource.PlaySound(sound.first);
                            }

                            //for (auto channel : setting.channels) {
                            //    FMOD_VECTOR fmodPosition = { setting.position.x, setting.position.y, setting.position.z };
                            //    channel->set3DAttributes(&fmodPosition, 0);
                            //}
                        }
                    }
                }
                {
                    auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::Video, Component::Transform>() };
                    for (auto entity : rbsystem) {
                        auto& video{ rbsystem.get<Component::Video>(entity) };
                        if (mgr.mSceneStarted && mgr.mScenePaused) {
                            mgr.mGroup[video.channelGroup]->setPaused(false);
                        }
                        if (mgr.mSceneStarted && mgr.mSceneStopped) {
                            if (video.IsAudioEnabled() && video.playOnStart) {
                                mgr.PlaySound(video.sound, video.audioPlaySettings, video.channelGroup, "video");
                                if (mgr.mGroup.find(video.channelGroup) != mgr.mGroup.end()) {
                                    FMOD::ChannelGroup* videoSoundGrp = mgr.mGroup[video.channelGroup];
                                    int numChannels = 0;
                                    videoSoundGrp->getNumChannels(&numChannels);
                                    for (int i = 0; i < numChannels; ++i) {
                                        FMOD::Channel* channel = nullptr;
                                        videoSoundGrp->getChannel(i, &channel);
                                        if (channel) {
                                            unsigned int pos = video.audioOffset; // jump 5 seconds into the playback
                                            FMOD_RESULT result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                mgr.mSceneStarted = false;
                mgr.mSceneStopped = false;
            }
            else if (mgr.mScenePaused == true) {
                auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                for (auto entity : rbsystem) {
                    auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                    mgr.mGroup[audiosource.channelGroup]->setPaused(true);
                }
            }
            else {
                mgr.mSceneStopped = true;
                static bool firsttime{ true };
                if (firsttime) {
                    auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                    FMOD::ChannelGroup* master{};
                    
                    FMOD_RESULT result{ mgr.mSystem->getMasterChannelGroup(&master) };
                    if (result == FMOD_OK) {
                        master->stop();
                    }
                    else {
                        Debug::DebugLogger::GetInstance().LogWarning("failed to stop all sounds");
                    }
                    firsttime = false;
                }
            }

            FMOD_RESULT result = mgr.mSystem->update();
            if (result != FMOD_OK) {
                Debug::DebugLogger::GetInstance().LogError(std::string("FMOD Error: ") + FMOD_ErrorString(result));
            }
        }
	}
}
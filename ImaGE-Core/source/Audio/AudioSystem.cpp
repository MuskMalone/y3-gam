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
            {
                auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                for (auto entity : rbsystem) {
                    auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                    auto& xfm{ rbsystem.get<Component::Transform>(entity) };
                    for (auto& sound : audiosource.sounds) {
                        auto& setting{ sound.second.playSettings };
                        setting.position = xfm.worldPos;

                        for (auto channel : sound.second.playSettings.channels) {
                            channel->setPitch(sound.second.playSettings.pitch);
                            if (sound.second.playSettings.mute) {
                                channel->setVolume(0);
                            }
                            else {
                                channel->setVolume(sound.second.playSettings.volume);
                            }

                        }
                    }
                }
            }
            {//only accounts for one audio listener
                auto rbsystem{ mEntityManager.GetAllEntitiesWithComponents<Component::AudioListener, Component::Transform, Component::Camera>() };
                //gets the first listener
                if (rbsystem.size_hint() > 0) {
                    ECS::Entity e{ rbsystem.front() };
                    auto& xfm{ e.GetComponent<Component::Transform>() };
                    auto& camera{ e.GetComponent<Component::Camera>() };
                    auto forwardVec{ camera.GetForwardVector() };
                    auto upVec{ camera.GetUpVector() };
                    // Listener setup
                    FMOD_VECTOR listenerPos = { xfm.position.x, xfm.position.y, xfm.position.z };
                    FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
                    FMOD_VECTOR listenerForward = { forwardVec.x, forwardVec.y, forwardVec.z };
                    FMOD_VECTOR listenerUp = { upVec.x, upVec.y, upVec.z };
                    //mgr.mSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
                }

            }
            //auto rbsystem2{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioListener, Component::Transform>() };
            //mgr.mSystem->set3DNumListeners(rbsystem2.)
            //for (auto entity : rbsystem2) {
            //    
            //}
            if (Scenes::SceneManager::GetInstance().GetSceneState() == Scenes::SceneState::PLAYING) {
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
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
            auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
            for (auto entity : rbsystem) {
                auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                auto& xfm{ rbsystem.get<Component::Transform>(entity) };
                for (auto& sound : audiosource.sounds) {
                    auto& setting{ sound.second.playSettings };
                    setting.position = xfm.worldPos;
                }
            }
            //auto rbsystem2{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioListener, Component::Transform>() };
            //mgr.mSystem->set3DNumListeners(rbsystem2.)
            //for (auto entity : rbsystem2) {
            //    
            //}
            if (Scenes::SceneManager::GetInstance().GetSceneState() == Scenes::SceneState::PLAYING) {
                auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                for (auto entity : rbsystem) {
                    auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                    auto& xfm{ rbsystem.get<Component::Transform>(entity) };
                    if (mgr.mSceneStarted && mgr.mScenePaused) {
                        mgr.mGroup[audiosource.channelGroup]->setPaused(false);
                    }
                    for (auto& sound : audiosource.sounds) {
                        auto& setting{ sound.second.playSettings };
                        if (mgr.mSceneStarted && mgr.mSceneStopped && setting.playOnAwake) {
                            audiosource.PlaySound(sound.first);
                        }
                        
                        for (auto channel : setting.channels) {
                            FMOD_VECTOR fmodPosition = { setting.position.x, setting.position.y, setting.position.z };
                            channel->set3DAttributes(&fmodPosition, 0);
                        }
                    }
                }
                mgr.mSceneStarted = false;
                mgr.mSceneStopped = false;
            }
            else if (mgr.mScenePaused == true) {
                auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
                for (auto entity : rbsystem) {
                    auto& audiosource{ rbsystem.get<Component::AudioSource>(entity) };
                    auto& xfm{ rbsystem.get<Component::Transform>(entity) };
                    mgr.mGroup[audiosource.channelGroup]->setPaused(true);
                }
            }
            else {
                mgr.mSceneStopped = true;
                static bool firsttime{ true };
                if (firsttime) {
                    auto rbsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::AudioSource, Component::Transform>() };
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
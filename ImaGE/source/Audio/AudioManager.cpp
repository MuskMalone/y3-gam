#include <pch.h>
#include "AudioManager.h"
#include <iostream>
#include <filesystem>
#include <fmod_errors.h>

bool AudioManager::Initialize() {
    FMOD_RESULT result;
    system = nullptr;

    result = FMOD::System_Create(&system); // Create the main system object.
    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return false;
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Creation", true);
    }

    // Initialize FMOD with 3D capabilities
    result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);

    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return false;
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Creation", true);
    }

    system->getMasterChannelGroup(&group["Master"]);
    CreateGroup("SFX");
    CreateGroup("BGM");
    CreateGroup("ENV");
    CreateGroup("VOC");

    UpdateAudioDirectory();

    return true;
}

void AudioManager::Update() {
    // Update the FMOD system and check for errors
    FMOD_RESULT result = system->update();
    if (result != FMOD_OK)
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }

    // loop through sound channels and remove those that are no longer playing
    for (auto& c : channels) {
        std::vector<FMOD::Channel*> removeList{};
        for (auto& s : c.second) {
            bool isPlaying;
            s->isPlaying(&isPlaying);
            if (!isPlaying) {
                removeList.push_back(s);
            }
        }

        // Remove stopped channels from the list
        for (auto& s : removeList) {
            c.second.remove(s);
        }
    }
}

void AudioManager::Release() {
    FMOD_RESULT result;
    result = system->release();

    if (result != FMOD_OK) {
        //std::cout << "FMOD error! " << FMOD_ErrorString(result) << "\n";
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Shutdown", true);
    }
}

FMOD::ChannelGroup* AudioManager::CreateGroup(const char* name) {

    if (group.count(name)) {
        return group[name];
    }
    FMOD::ChannelGroup* channelgroup;
    FMOD_RESULT result;
    result = system->createChannelGroup(name, &channelgroup);
    group[name] = channelgroup;

    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD Channel Group Creation", true);
    }
    return group[name];
}

void AudioManager::SetGroupVolume(const char* name, float volume) {
    FMOD_RESULT result;
    if (group.find(name) != group.end()) 
    {
        result = group[name]->setVolume(volume);
    }

    if (result != FMOD_OK) {
        //std::cout << "FMOD error! " << FMOD_ErrorString(result) << "\n";
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Set group volume to " + std::to_string(volume), true);
    }
}

float AudioManager::GetGroupVolume(const char* name) {
    float volume = 0.0f;
    if (group.find(name) != group.end()) {
        FMOD_RESULT result;
        result = group[name]->getVolume(&volume);
        if (result != FMOD_OK) {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
    }
    else {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
    return volume;
}

void AudioManager::StopGroup(const char* name) {
    if (group.find(name) != group.end()) {
        FMOD_RESULT result;
        result = group[name]->stop();
        if (result != FMOD_OK) {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else {
            Debug::DebugLogger::GetInstance().LogInfo("Group Stopped", true);
        }
    }
    else {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
}

void AudioManager::ResumeGroup(const char* name) {
    if (group.find(name) != group.end()) {
        FMOD_RESULT result;
        result = group[name]->setPaused(false);
        if (result != FMOD_OK) {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else {
            Debug::DebugLogger::GetInstance().LogInfo("Resume Group", true);
        }
    }
    else {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
}

void AudioManager::PauseGroup(const char* name) {
    if (group.find(name) != group.end()) {
        FMOD_RESULT result;
        result = group[name]->setPaused(true);
        if (result != FMOD_OK) {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else {
            Debug::DebugLogger::GetInstance().LogInfo("Paused Group", true);
        }
    }
    else {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
}

bool AudioManager::IsGroupPaused(const char* name) {
    bool status = false;
    if (group.find(name) != group.end()) {
        FMOD_RESULT result;
        result = group[name]->getPaused(&status);
        if (result != FMOD_OK) {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
    }
    else {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
    return status;
}

FMOD::Sound* AudioManager::AddSound(const char* filepath, const char* name) {
    // Check if the sound already exists
    if (data.count(name) && data[name] != nullptr) {
        return data[name];
    }

    FMOD_RESULT result = system->createSound(filepath, FMOD_DEFAULT, 0, &data[name]);

    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    } else {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully added sound: " + std::string(filepath), true);
    }

    return data[name];
}


FMOD::Sound* AudioManager::AddMusic(const char* filepath, const char* name) {
    if (data[name] != nullptr) {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(filepath, FMOD_LOOP_NORMAL, 0, &data[name]);
    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully added music: " + std::string(filepath), true);
    }

    //If no BGM loaded, player current BGM
    if (originalBGM == "") {
        PlaySound(name, "BGM");
        originalBGM = name;
        currentBGM = name;
    }
    return data[name];
}

FMOD::Sound* AudioManager::AddAmbience(const char* filepath, const char* name) {
    if (data[name] != nullptr) {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(filepath, FMOD_LOOP_NORMAL, 0, &data[name]);
    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully added ambience: " + std::string(filepath), true);
    }
    //If no BGM loaded, player current BGM
    if (currentAmbience == "") {
        PlaySound(name, "ENV");
        currentAmbience = name;
    }
    return data[name];
}

FMOD::Channel* AudioManager::PlaySound(const char* sound, const char* channelGroup, int loops, FMOD_VECTOR* position) {
    FMOD::Channel* tmp = nullptr;
    FMOD_RESULT result;

    // Load the sound if it's not already loaded
    if (data[sound] == nullptr) {
        //assetmanager.LoadAssets(sound);
    }

    // Set looping count for the sound
    result = data[sound]->setLoopCount(loops);
    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return nullptr;
    }

    // Play the sound, handling 2D or 3D based on the position argument
    if (channelGroup == nullptr) {
        result = system->playSound(data[sound], 0, false, &tmp);
    }
    else {
        result = system->playSound(data[sound], group[channelGroup], false, &tmp);
        channels[channelGroup].push_back(tmp); // Add the channel to the group
    }

    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return nullptr;
    }

    // If a position is provided, treat it as a 3D sound and set its 3D attributes
    if (position != nullptr && tmp != nullptr) {
        tmp->set3DAttributes(position, nullptr);  // Setting 3D position
    }

    return tmp;
}

void AudioManager::FreeSound(const char* sound) {
    data[sound]->release();
    data.erase(sound);
}

void AudioManager::SetBGM(const char* name) {
    StopGroup("BGM");
    FreeSound(currentBGM.c_str());
    currentBGM = name;
    PlaySound(name, "BGM");
}

void AudioManager::RestartBGM() {
    SetBGM(originalBGM.c_str());
}

void AudioManager::SetAmbience(const char* name) {
    StopGroup("ENV");
    FreeSound(currentAmbience.c_str());
    currentAmbience = name;
    PlaySound(name, "ENV");
}

void AudioManager::ReleaseAllSounds() {
    for (auto const& sound : data) {
        sound.second->release();
    }
    data.clear();
    currentBGM.clear();
    originalBGM.clear();
    currentAmbience.clear();
}

FMOD::System* AudioManager::GetSystem() {
    return system;
}

std::vector<std::string> AudioManager::GetSoundNames() {
    std::vector<std::string> output{};
    for (auto& name : data) {
        output.push_back(name.first);
    }
    return output;
}

std::vector<std::string> AudioManager::GetSoundPaths() {
    return soundPaths;
}

std::vector<std::string> AudioManager::GetMusicPaths() {
    return musicPaths;
}

std::vector<std::string> AudioManager::GetAmbiencePaths() {
    return ambiencePaths;
}

std::string AudioManager::GetCurrentBGM() {
    return currentBGM;
}

std::string AudioManager::GetCurrentAmbience() {
    return currentAmbience;
}

void AudioManager::SetGroupFilter(const char* name, float filter) {
    for (auto& c : channels[name]) {
        c->setLowPassGain(filter);
    }
}

void AudioManager::SetPlayerAttributes(const FMOD_3D_ATTRIBUTES& attributes) {
    FMOD_RESULT result = system->set3DListenerAttributes(0, &attributes.position, &attributes.velocity, &attributes.forward, &attributes.up);

    if (result != FMOD_OK) {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully set 3D Player attributes", true);
    }
}

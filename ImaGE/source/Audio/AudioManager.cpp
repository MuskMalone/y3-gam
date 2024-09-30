#include <pch.h>
#include "AudioManager.h"
#include <iostream>
#include <filesystem>
#include <fmod_errors.h>

bool AudioManager::Initialize() 
{
    FMOD_RESULT result;
    system = nullptr;

    //creating the FMOD system object, inorder to use FMOD's functionalities
    result = FMOD::System_Create(&system); 
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
    result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);

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

    result = system->set3DListenerAttributes(0, &listenerPosition, &listenerVelocity, &forwardDirection, &upDirection);
    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Could not set listener attributes: " + str, true);
        return false;
    }

    system->set3DSettings(1.0f, 1.0f, 1.0f); // Distance factor, rolloff scale, Doppler scale
    //fetching the master channel group, to control the volume and other properties of all sounds globally
    system->getMasterChannelGroup(&group["Master"]);
    CreateGroup("SFX");
    CreateGroup("BGM");
    CreateGroup("ENV");
    CreateGroup("VOC");

    UpdateAudioDirectory();

    return true;
}

void AudioManager::Update() 
{
    // update the FMOD system for real-time changes
    FMOD_RESULT result = system->update();
    if (result != FMOD_OK)
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }

    // loop through sound channels and adds those that are no longer playing to the remove list
    for (auto& c : channels) 
    {
        std::vector<FMOD::Channel*> removeList{};
        for (auto& s : c.second) 
        {
            bool isPlaying;
            s->isPlaying(&isPlaying);
            if (!isPlaying)
            {
                removeList.push_back(s);
            }
        }
        // remove stopped channels from the list
        for (auto& s : removeList) 
        {
            c.second.remove(s);
        }
    }
}

void AudioManager::Release() 
{
    FMOD_RESULT result;
    //clear the entire FMOD system when the game shuts down
    result = system->release();

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD System Shutdown", true);
    }
}

FMOD::ChannelGroup* AudioManager::CreateGroup(const char* name) 
{
    //checks if a channel group with the given name already exists in the group map
    //count() return 1 if the group exist
    if (group.count(name)) 
    {
        return group[name];//if exist, return that group to avoid duplicate group creation
    }
    //when group not exist
    FMOD::ChannelGroup* channelgroup;
    FMOD_RESULT result;
    result = system->createChannelGroup(name, &channelgroup);//creates a new channel group with that name
    group[name] = channelgroup;//stores the new channel group in the group map, using name as the key

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Successful FMOD Channel Group Creation", true);
    }
    return group[name];//return the channel group
}

void AudioManager::SetGroupVolume(const char* name, float volume) 
{
    //checks if the given group(by name) exists in the group map
    if (group.find(name) != group.end()) 
    {
        FMOD_RESULT result;
        result = group[name]->setVolume(volume);//if it exists, sets the volume of the group
        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Set group volume to " + std::to_string(volume), true);
    }
}

float AudioManager::GetGroupVolume(const char* name) 
{
    float volume = 0.0f;
    //checks if the given group(by name) exists in the group map
    if (group.find(name) != group.end()) 
    {
        FMOD_RESULT result;
        //if exist, it gets the the current volume of the channel group and store in volume
        result = group[name]->getVolume(&volume);
        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else 
        {
            Debug::DebugLogger::GetInstance().LogInfo("Group Volume is" + std::to_string(volume), true);
        }
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
    return volume;
}

void AudioManager::StopGroup(const char* name) 
{
    if (group.find(name) != group.end()) 
    {
        FMOD_RESULT result;
        result = group[name]->stop();
        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else 
        {
            Debug::DebugLogger::GetInstance().LogInfo("Stop Group", true);
        }
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
}

void AudioManager::ResumeGroup(const char* name) 
{
    if (group.find(name) != group.end()) 
    {
        FMOD_RESULT result;
        result = group[name]->setPaused(false);
        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else 
        {
            Debug::DebugLogger::GetInstance().LogInfo("Resume Group", true);
        }
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
}

void AudioManager::PauseGroup(const char* name) 
{
    if (group.find(name) != group.end()) 
    {
        FMOD_RESULT result;
        result = group[name]->setPaused(true);
        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        else 
        {
            Debug::DebugLogger::GetInstance().LogInfo("Pause Group", true);
        }
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD error! Group not found: " + std::string(name), true);
    }
}

bool AudioManager::IsGroupPaused(const char* name) 
{
    bool status = false;
    //checks if the given group(by name) exists in the group map
    if (group.find(name) != group.end()) 
    {
        FMOD_RESULT result;
        //if exist, get the pause state and store in status
        result = group[name]->getPaused(&status);
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

FMOD::Sound* AudioManager::AddSound(const char* filepath, const char* name) 
{
    //checks if the sound already exist
    if (data.find(name) != data.end() && data[name] != nullptr) 
    {
        return data[name];
    }
   
    //FMOD_RESULT result = system->createSound(filepath, FMOD_DEFAULT, 0, &data[name]); //non-3D sound
    FMOD_RESULT result = system->createSound(filepath, FMOD_3D, 0, &data[name]);//create 3D sound

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    } 
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully added sound: " + std::string(filepath), true);
    }

    return data[name];
}

FMOD::Sound* AudioManager::AddMusic(const char* filepath, const char* name) 
{
    if (data[name] != nullptr) 
    {
        return data[name];
    }

    FMOD_RESULT result;
    result = system->createSound(filepath, FMOD_LOOP_NORMAL, 0, &data[name]);//loads the music with looping enabled

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    } 
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully added music: " + std::string(filepath), true);
    }

    // If no BGM loaded, play current BGM
    if (originalBGM == "") 
    {
        PlaySound(name, "BGM");
        originalBGM = name;
        currentBGM = name;
    }
    return data[name];
}


FMOD::Sound* AudioManager::AddAmbience(const char* filepath, const char* name) 
{
    if (data[name] != nullptr) 
    {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(filepath, FMOD_3D | FMOD_LOOP_NORMAL, 0, &data[name]);//make the ambience 3D

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully added ambience: " + std::string(filepath), true);
    }
    // if no ambient sound loaded, play current ambient sound
    if (currentAmbience == "") 
    {
        PlaySound(name, "ENV");
        currentAmbience = name;
    }
    return data[name];
}

FMOD::Channel* AudioManager::PlaySound(const char* sound, const char* channelGroup, int loops, FMOD_VECTOR* position) 
{
    FMOD_RESULT result;

    //checks if the sound is loaded, if not loaded, it could load using an asset manager
    //asset manager loads sounds into data[sound]
    if (data[sound] == nullptr) 
    {
        //assetmanager.LoadAssets(sound);
    }

    //setting the loop count for the sound
    //loop value = -1 means the sound will loop indefinitely
    result = data[sound]->setLoopCount(loops);
    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return nullptr;
    }

    FMOD::Channel* temp = nullptr;
    //Plays the sound
    //if no channelGroup is provided, play the sound without grouping it. 
    if (channelGroup == nullptr) 
    {
        result = system->playSound(data[sound], 0, false, &temp);
    }
    else 
    {
        //Otherwise, assigns the sound to the specified channel group (e.g., BGM, SFX, etc.)
        result = system->playSound(data[sound], group[channelGroup], false, &temp);
        //when sound is added to a channel group, we also adds the channel reference in the channels map.
        channels[channelGroup].push_back(temp); 
    }

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return nullptr;
    }

    // if a position is provided, treat it as a 3D sound
    if (position != nullptr && temp != nullptr) 
    {
        // Set the 3D attributes (position), specifies where the sound is located in the 3D world relative to the player
        temp->set3DAttributes(position, nullptr);

        // Set the 3D min/max distance for sound attenuation
        float minDistance = 1.0f;   // start attenuation at 1 unit
        float maxDistance = 100.0f; // full attenuate at 100 units
        result = temp->set3DMinMaxDistance(minDistance, maxDistance);

        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        Debug::DebugLogger::GetInstance().LogInfo("Playing 3D sound: " + std::string(sound), true);
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Playing 2D sound: " + std::string(sound), true);
    }

    return temp;
}

FMOD::Channel* AudioManager::PlaySound(const char* sound, const char* channelGroup = nullptr, int loops = 0, FMOD_VECTOR* position = nullptr, float minDistance, float maxDistance)
{
    FMOD_RESULT result;

    //checks if the sound is loaded, if not loaded, it could load using an asset manager
    //asset manager loads sounds into data[sound]
    if (data[sound] == nullptr) 
    {
        //assetmanager.LoadAssets(sound);
    }

    //setting the loop count for the sound
    //loop value = -1 means the sound will loop indefinitely
    result = data[sound]->setLoopCount(loops);
    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return nullptr;
    }

    FMOD::Channel* temp = nullptr;
    //Plays the sound
    //if no channelGroup is provided, play the sound without grouping it. 
    if (channelGroup == nullptr) 
    {
        result = system->playSound(data[sound], 0, false, &temp);
    }
    else 
    {
        //Otherwise, assigns the sound to the specified channel group (e.g., BGM, SFX, etc.)
        result = system->playSound(data[sound], group[channelGroup], false, &temp);
        //when sound is added to a channel group, we also adds the channel reference in the channels map.
        channels[channelGroup].push_back(temp);
    }

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        return nullptr;
    }

    // if a position is provided, treat it as a 3D sound
    if (position != nullptr && temp != nullptr) 
    {
        // Set the 3D attributes (position), specifies where the sound is located in the 3D world relative to the player
        temp->set3DAttributes(position, nullptr);
        // Set the 3D min/max distance for attenuation
        result = tmp->set3DMinMaxDistance(minDistance, maxDistance);

        if (result != FMOD_OK) 
        {
            std::string str(FMOD_ErrorString(result));
            Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
        }
        Debug::DebugLogger::GetInstance().LogInfo("Playing 3D sound: " + std::string(sound), true);
    }
    else
    {
        Debug::DebugLogger::GetInstance().LogInfo("Playing 2D sound: " + std::string(sound), true);
    }

    return temp;
}

void AudioManager::FreeSound(const char* sound) 
{
    //freeing a specific sound from memory 
    //1.releasing the sound in FMOD 2. removing it from the data map.
    if (data.find(sound) != data.end() && data[sound] != nullptr) 
    {
        data[sound]->release();
        data.erase(sound);
    } 
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Freeing non-existent sound: " + std::string(sound), true);
    }
}

void AudioManager::SetBGM(const char* name) 
{
    StopGroup("BGM"); //stops all sounds currently playing in BGM group
    if (!currentBGM.empty()) 
    {
        FreeSound(currentBGM.c_str());//frees the memory of the previously playing BGM
    }
    currentBGM = name;//sets the new BGM name
    PlaySound(name, "BGM"); //plays new BGM
}

void AudioManager::RestartBGM() 
{
    if (!originalBGM.empty()) 
    {
        SetBGM(originalBGM.c_str());//simply calls Setbgm with the original/previous BGM name
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! No original BGM to restart.", true);
    }
}

void AudioManager::SetAmbience(const char* name) 
{
    StopGroup("ENV");
    if (!currentAmbience.empty()) 
    {
        FreeSound(currentAmbience.c_str());
    }
    currentAmbience = name;
    PlaySound(name, "ENV");
}

void AudioManager::ReleaseAllSounds() 
{
    size_t soundCount = 0;
    //loop releases each sound stored in the data map
    for (auto const& sound : data) 
    {
        //ensure no nullptr sounds are released
        if (sound.second != nullptr) 
        {
            sound.second->release();
            ++soundCount;
        }
    }
    //after releasing the sounds, the data map is cleared 
    data.clear();
    currentBGM.clear();
    originalBGM.clear();
    currentAmbience.clear();

    Debug::DebugLogger::GetInstance().LogInfo("Successfully released " + std::to_string(soundCount) + " sounds.", true);
}

FMOD::System* AudioManager::GetSystem() 
{
    return system;
}

std::vector<std::string> AudioManager::GetSoundNames() 
{
    std::vector<std::string> output{};
    //iterates over the data map
    for (auto& pair : data) 
    {
        output.push_back(pair.first);//pair.first gives the keys(which are sound names), and add to output
    }
    return output;
}

std::vector<std::string> AudioManager::GetSoundPaths() 
{
    return soundPaths;
}

std::vector<std::string> AudioManager::GetMusicPaths() 
{
    return musicPaths;
}

std::vector<std::string> AudioManager::GetAmbiencePaths() 
{
    return ambiencePaths;
}

std::string AudioManager::GetCurrentBGM() 
{
    return currentBGM;
}

std::string AudioManager::GetCurrentAmbience() 
{
    return currentAmbience;
}

void AudioManager::SetGroupFilter(const char* name, float filter) 
{
    // check if the group exists before applying the filter
    if (channels.find(name) != channels.end()) 
    {
        //apply low-pass filter to all sound within a specific channel group
        //access the list of channels associated with the group name
        for (auto& c : channels[name]) 
        {
            FMOD_RESULT result;
            result = c->setLowPassGain(filter);

            if (result != FMOD_OK) 
            {
                std::string str(FMOD_ErrorString(result));
                Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Could not set low-pass filter for channel: " + str, true);
            }
        }
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! Channel group not found: " + std::string(name), true);
    }
}

void AudioManager::SetPlayerAttributes(const FMOD_3D_ATTRIBUTES& attributes) 
{
    FMOD_RESULT result;
    //updates the 3D attributes of the listener.
    result = system->set3DListenerAttributes(0, &attributes.position, &attributes.velocity, &attributes.forward, &attributes.up);

    if (result != FMOD_OK) 
    {
        std::string str(FMOD_ErrorString(result));
        Debug::DebugLogger::GetInstance().LogError("FMOD ERROR! " + str, true);
    }
    else 
    {
        Debug::DebugLogger::GetInstance().LogInfo("Successfully set 3D Player attributes", true);
        Debug::DebugLogger::GetInstance().LogInfo("Position: {" + std::to_string(attributes.position.x) + ", " + std::to_string(attributes.position.y) + ", " + std::to_string(attributes.position.z) + "}", true);
        Debug::DebugLogger::GetInstance().LogInfo("Velocity: {" + std::to_string(attributes.velocity.x) + ", " + std::to_string(attributes.velocity.y) + ", " + std::to_string(attributes.velocity.z) + "}", true);
        Debug::DebugLogger::GetInstance().LogInfo("Forward: {" + std::to_string(attributes.forward.x) + ", " + std::to_string(attributes.forward.y) + ", " + std::to_string(attributes.forward.z) + "}", true);
        Debug::DebugLogger::GetInstance().LogInfo("Up: {" + std::to_string(attributes.up.x) + ", " + std::to_string(attributes.up.y) + ", " + std::to_string(attributes.up.z) + "}", true);
    }
}

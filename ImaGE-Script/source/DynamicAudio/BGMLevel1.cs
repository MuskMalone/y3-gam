using System;
using IGE.Utils;
using System.Numerics;

public class BGMLevel1 : Entity
{
    public Entity player;
    public Entity doorEntryTrigger;
    public Entity unlockDoorUI;
    public float distanceFromPlayerFromDoor;
    public bool isEnteredHallway = false;

    public float bgmVolume = 0.3f;
    public float ambienceVolume = 0.5f;
    public float hallwayAmbienceVolume = 0.0f;

    public float bgmVolBefore = 0.3f;
    public float bgmVolAfter = 0.05f;

    public float ambientVolBefore = 0.5f;
    public float ambientVolAfter = 0.0f;

    public float hallwayAmbientVolBefore = 0.0f;
    public float hallwayAmbientVolAfter = 1.0f;

    public float transitionTiming = 0.8f; // in seconds
    public float currTiming = 0.0f;
    
    public bool startPlaying;

    private bool playedNighttimeAmbience;

    void Start() {
        Global.isNighttime = false;
    }

    void Update()
    {
        
        InternalCalls.SetSoundVolume(player.mEntityID, "BGM", bgmVolume);
        if (Global.isNighttime)
        {
            InternalCalls.SetSoundVolume(player.mEntityID, "NighttimeAmbience", ambienceVolume);
            InternalCalls.SetSoundVolume(player.mEntityID, "Ambience", 0.0f);
        }
        else
        {
            InternalCalls.SetSoundVolume(player.mEntityID, "NighttimeAmbience", 0.0f);
            InternalCalls.SetSoundVolume(player.mEntityID, "Ambience", ambienceVolume);
        }
        InternalCalls.SetSoundVolume(player.mEntityID, "HallwayAmbience", hallwayAmbienceVolume);
        if (!unlockDoorUI.IsActive())
        {
            Vector3 playerLoc = InternalCalls.GetWorldPosition(player.mEntityID);
            Vector3 doorLoc = InternalCalls.GetWorldPosition(doorEntryTrigger.mEntityID);

            distanceFromPlayerFromDoor = (doorLoc - playerLoc).Length();

            isEnteredHallway = playerLoc.X > doorLoc.X;

            if (InternalCalls.OnTriggerExit(doorEntryTrigger.mEntityID, player.mEntityID)){
                startPlaying = true;
            }
            if (startPlaying)
            {
                if (currTiming <= transitionTiming)
                {
                    currTiming += InternalCalls.GetDeltaTime();
                    float percent = currTiming / transitionTiming;
                    if (isEnteredHallway)
                    {
                        bgmVolume = Easing.Linear(bgmVolBefore, bgmVolAfter, percent);
                        ambienceVolume = Easing.Linear(ambientVolBefore, ambientVolAfter, percent);
                        hallwayAmbienceVolume = Easing.Linear(hallwayAmbientVolBefore, hallwayAmbientVolAfter, percent);
                    }
                    else
                    {
                        bgmVolume = Easing.Linear(bgmVolAfter, bgmVolBefore, percent);
                        ambienceVolume = Easing.Linear(ambientVolAfter, ambientVolBefore, percent);
                        hallwayAmbienceVolume = Easing.Linear(hallwayAmbientVolAfter, hallwayAmbientVolBefore, percent);
                    }
                }
                else {
                    currTiming = 0.0f;
                    startPlaying = false;
                }
            }
        }
    }
}

using System;
using IGE.Utils;
using System.Numerics;

public class BGMLevel3 : Entity
{
  public Entity player;
  public Entity entryToPitTrigger;
  public Entity entryToMainAreaTrigger;
  public Entity mainAreaToRoomTrigger;
  public Entity mainAreaToExitTrigger;

  public Entity deathTrigger;

  public float bgmVolume = 0.3f;
  public float ambienceVolume = 0.5f;
  public float ambienceVolumeMultiplier = 1f;
  public float pitAmbienceVolume = 0.0f;
  public float pitAmbienceMultiplier = 1f;

  public float bgmVolBefore = 0.3f;
  public float bgmVolAfter = 0.05f;

  public float ambientVolBefore = 0.5f;
  public float ambientVolAfter = 0.0f;
  public float ambientVolAfterPit = 0.15f;

  public float pitAmbientVolBefore = 0.0f;
  public float pitAmbientVolAfter = 1.0f;

  public float transitionTiming = 0.8f; // in seconds
  public float currTiming = 0.0f;

  public float dist = 0f;

  public float pitLoudnessThreshold = 5f;
  public float pitLowestVolume = 0.1f;
  public float pitHighestVolume = 1f;
  private bool pitRoomStartLerp = false;
  void Start()
  {
  }

  void Update()
  {

    InternalCalls.SetSoundVolume(player.mEntityID, "BGM", bgmVolume);
    InternalCalls.SetSoundVolume(player.mEntityID, "Ambience", ambienceVolume);
    InternalCalls.SetSoundVolume(player.mEntityID, "PitAmbience", pitAmbienceVolume);
    Vector3 playerLoc = InternalCalls.GetWorldPosition(player.mEntityID);
    { //for pit room
      
      Vector3 doorLoc = InternalCalls.GetWorldPosition(entryToPitTrigger.mEntityID);
      bool isEntered = playerLoc.Z < doorLoc.Z;

      if (InternalCalls.OnTriggerExit(entryToPitTrigger.mEntityID, player.mEntityID))
      {
        pitRoomStartLerp = true;

      }
      if (pitRoomStartLerp)
      {
        if (currTiming <= transitionTiming)
        {
          currTiming += InternalCalls.GetDeltaTime();
          float percent = currTiming / transitionTiming;
          if (isEntered)
          {
            ambienceVolume = Easing.Linear(ambientVolBefore, ambientVolAfterPit, percent);
          }
          else
          {
            ambienceVolume = Easing.Linear(ambientVolAfterPit, ambientVolBefore, percent);
          }
        }
        else
        {
          currTiming = 0.0f;
          pitRoomStartLerp = false;
        }

      }
      dist = InternalCalls.GetShortestDistance(player.mEntityID, deathTrigger.mEntityID);
      if (isEntered ) // for pit Reverb
      {
        pitAmbienceVolume = (pitLoudnessThreshold - dist) / pitLoudnessThreshold;
        if (pitAmbienceVolume < pitLowestVolume) pitAmbienceVolume = pitLowestVolume;
        if (pitAmbienceVolume > pitHighestVolume) pitAmbienceVolume = pitHighestVolume;
      }else
      {
        pitAmbienceVolume = 0f;
      }

    }
  }
}

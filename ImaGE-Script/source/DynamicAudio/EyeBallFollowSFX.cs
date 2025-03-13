using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class EyeBallFollowSFX : Entity
{
  public EyeBallFollow eyeball; // can be eyeball1 or eyeball 2
  public float vol = 1f;
  void Start()
  {

  }
  void Update()
  {
    if (eyeball.isFollowing)
    {
      InternalCalls.SetSoundVolume(mEntityID, "EyeballSquish2", vol);
    }
    else
    {
      InternalCalls.SetSoundVolume(mEntityID, "EyeballSquish2", 0.0f);
    }
  }
}
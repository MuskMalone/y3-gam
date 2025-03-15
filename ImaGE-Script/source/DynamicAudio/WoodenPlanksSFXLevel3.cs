using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using static Dialogue;

public class WoodenPlankSFXLevel3 : Entity
{
  public bool pair1Exists = false;
  public bool pair2Exists = false;
  public Entity otherPlank1;
  public Entity otherPlank2;
  public Entity floor;

  private float lowerImpulseThreshold = 3f;
  private float upperImpulseThreshold = 31f;

  int heavyCount = 0;
  int lightCount = 0;
  int moderateCount = 0;
  void Start()
  {


  }
  void PlayHeavySound()
  {
    heavyCount %= 3;
    InternalCalls.PlaySound(mEntityID, $"..\\Assets\\Audio\\PlankHitHeavy{heavyCount+1}_SFX.wav");
    heavyCount++;
  }
  void PlayLightSound() {
    lightCount %= 3;
    InternalCalls.PlaySound(mEntityID, $"..\\Assets\\Audio\\PlankHitLight{lightCount + 1}_SFX.wav");
    lightCount++;
  }
  void PlayModerateSound()
  {
    moderateCount %= 4;
    InternalCalls.PlaySound(mEntityID, $"..\\Assets\\Audio\\PlankHit{moderateCount + 1}_SFX.wav");
    moderateCount++;
  }
  void PlaySound(float f)
  {
    float incr = (upperImpulseThreshold - lowerImpulseThreshold) / 3f;
    if (f > lowerImpulseThreshold && f < lowerImpulseThreshold + incr)
    {
      PlayLightSound();
    }
    else if (f > lowerImpulseThreshold + incr && f < lowerImpulseThreshold + incr + incr)
    {
      PlayModerateSound();
    }
    else if (f > lowerImpulseThreshold + incr + incr)
    {
      PlayHeavySound();
    }
  }
  void Update()
  {
    if (pair1Exists)
    {
      ContactPoint[] withOtherPlank1 = InternalCalls.GetContactPoints(mEntityID, otherPlank1.mEntityID);

      HashSet<float> uniqueMagnitudes = new HashSet<float>();

      foreach (ContactPoint cp in withOtherPlank1)
      {
        float magnitude = cp.impulse.LengthSquared();
        float roundedMagnitude = (float)Math.Round(magnitude, 5);

        if (!uniqueMagnitudes.Contains(roundedMagnitude) && roundedMagnitude > lowerImpulseThreshold)
        {
          uniqueMagnitudes.Add(roundedMagnitude);
          Debug.Log($"play plank sound from {InternalCalls.GetTag(otherPlank1.mEntityID)} with mag: {roundedMagnitude}");
          PlaySound(roundedMagnitude);
        }
      }
    }

    if (pair2Exists)
    {
      ContactPoint[] withOtherPlank2 = InternalCalls.GetContactPoints(mEntityID, otherPlank2.mEntityID);

      HashSet<float> uniqueMagnitudes = new HashSet<float>();

      foreach (ContactPoint cp in withOtherPlank2)
      {
        float magnitude = cp.impulse.LengthSquared();
        float roundedMagnitude = (float)Math.Round(magnitude, 5);

        if (!uniqueMagnitudes.Contains(roundedMagnitude) && roundedMagnitude > lowerImpulseThreshold)
        {
          uniqueMagnitudes.Add(roundedMagnitude);
          Debug.Log($"play plank sound from {InternalCalls.GetTag(otherPlank2.mEntityID)} with mag: {roundedMagnitude}");
          PlaySound(roundedMagnitude);
        }
      }
    }

    {
      ContactPoint[] withFloor = InternalCalls.GetContactPoints(mEntityID, floor.mEntityID);

      HashSet<float> uniqueMagnitudes = new HashSet<float>();

      foreach (ContactPoint cp in withFloor)
      {
        float magnitude = cp.impulse.LengthSquared();
        float roundedMagnitude = (float)Math.Round(magnitude, 5);

        if (!uniqueMagnitudes.Contains(roundedMagnitude) && roundedMagnitude > lowerImpulseThreshold)
        {
          uniqueMagnitudes.Add(roundedMagnitude);
          Debug.Log($"play plank sound from {InternalCalls.GetTag(mEntityID)} hitting floor with mag: {roundedMagnitude}");
          PlaySound(roundedMagnitude);
        }
      }
    }
  }


}

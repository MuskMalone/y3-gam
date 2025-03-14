using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using static Dialogue;

public class WoodenPlankSFXLevel3 : Entity
{
  public bool pair1Exists = false;
  public bool pair2Exists = false;
  public Entity otherPlank1;
  public Entity otherPlank2;
  public Entity floor;

  private float lowerImpulseThreshold = 3f;
  private float upperImpulseThreshold = 10f;
  void Start()
  {


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
          InternalCalls.PlaySound(mEntityID, "PlankClack1_SFX.wav");
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
          InternalCalls.PlaySound(mEntityID, "PlankClack1_SFX.wav");
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
          InternalCalls.PlaySound(mEntityID, "PlankClack1_SFX.wav");
        }
      }
    }
  }


}

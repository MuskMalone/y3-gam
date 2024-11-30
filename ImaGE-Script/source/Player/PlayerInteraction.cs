using IGE.Utils;
using System;
using System.Numerics;

public class PlayerInteraction : Entity
{
  public float RayFromPlayerCameraLength;
  public string RayHitString;

  void Start()
  {
    RayHitString = string.Empty;
  }

  void Update()
  {
    Vector3 rayOrigin = InternalCalls.GetMainCameraPosition(FindEntityByTag("MainCamera").mEntityID);
    Vector3 rayDirection = InternalCalls.GetMainCameraDirection(FindEntityByTag("MainCamera").mEntityID);
    Vector3 rayEnd = rayOrigin + (rayDirection * RayFromPlayerCameraLength);

    uint rayHitID = InternalCalls.RaycastFromEntity(mEntityID, rayOrigin, rayEnd);

        if (rayHitID != 0)
        {
            RayHitString = InternalCalls.GetTag(rayHitID);
            Console.WriteLine(RayHitString);
        }
        else
            RayHitString = "None";
  }
}
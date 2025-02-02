using IGE.Utils;
using System;

public class LightSwitch : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity[] LightsToToggleActive;

  public LightSwitch() : base()
  {

  }

  void Start()
  {

  }

  void Update()
  {
    bool mouseClicked = Input.GetMouseButtonTriggered(0);
    bool isSwitchHit = playerInteraction.RayHitString == "Light Switch";

    if (mouseClicked)
    {
      if (isSwitchHit)
      {
                Console.WriteLine("CameHere");    
        foreach (Entity light in LightsToToggleActive)
        {
          light.SetActive(!InternalCalls.IsActive(light.mEntityID));
        }
      }
    }
  }
}
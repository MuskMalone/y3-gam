using IGE.Utils;

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
        foreach (Entity light in LightsToToggleActive)
        {
          light.SetActive(!InternalCalls.IsActive(light.mEntityID));
        }
      }
    }
  }
}
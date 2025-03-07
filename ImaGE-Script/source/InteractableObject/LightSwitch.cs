using IGE.Utils;
using System;

public class LightSwitch : Entity
{
    public PlayerInteraction playerInteraction;
    public ControlPanel2 controlPanel;
    public Entity[] LightsToToggleActive;

    private bool lightsOn = true;
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

        if (mouseClicked && isSwitchHit)
        {
            //Console.WriteLine("CameHere");
            InternalCalls.PlaySound(mEntityID, "LightSwitch");

            lightsOn = !lightsOn;
            foreach (Entity light in LightsToToggleActive)
            {
                light.SetActive(lightsOn);
            }

            controlPanel.LightsToggled(lightsOn);
        }
    }
}
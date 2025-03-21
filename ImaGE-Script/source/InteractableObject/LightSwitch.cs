using IGE.Utils;
using System;

public class LightSwitch : Entity
{
    public PlayerInteraction playerInteraction;
    public ControlPanel2 controlPanel;
    public Entity[] LightsToToggleActive;
    public Entity lever;

    private bool playingAnim = false;
    private bool lightsOn = true;
    public LightSwitch() : base()
    {

    }

    void Start()
    {

    }

    void Update()
    {
        if (playingAnim)
        {
            if (InternalCalls.IsPlayingAnimation(lever.mEntityID))
            {
                return;
            }

            // when animation has ended, toggle lights
            lightsOn = !lightsOn;
            foreach (Entity light in LightsToToggleActive)
            {
                light.SetActive(lightsOn);
            }

            controlPanel.LightsToggled(lightsOn);
            playingAnim = false;
        }
        else
        {
            bool mouseClicked = Input.GetMouseButtonTriggered(0);
            bool isSwitchHit = playerInteraction.RayHitString == "Light Switch";

            if (mouseClicked && isSwitchHit)
            {
                //Console.WriteLine("CameHere");
                InternalCalls.PlaySound(mEntityID, lightsOn ? "LeverUp" : "LeverDown");

                InternalCalls.PlayAnimation(lever.mEntityID, lightsOn ? "SwitchOff" : "SwitchOn");
                playingAnim = true;
            }
        }
    }
}
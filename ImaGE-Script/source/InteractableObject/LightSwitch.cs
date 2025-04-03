using IGE.Utils;
using System;

public class LightSwitch : Entity
{
    public PlayerInteraction playerInteraction;
    public ControlPanel2 controlPanel;
    public Entity[] LightsToToggleActive;
    public Entity lever;
    public Entity EToInteractUI;

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
            if (lever.GetComponent<Animation>().IsPlaying())
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

                lever.GetComponent<Animation>().Play(lightsOn ? "SwitchOff" : "SwitchOn");
                playingAnim = true;
                EToInteractUI.SetActive(false);
                return;
            }

            EToInteractUI.SetActive(isSwitchHit);
        }
    }
}
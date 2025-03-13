using IGE.Utils;
using System;

public class PullLever : Entity
{
    public PlayerInteraction playerInteraction;
    private bool leverPulled = false; // Track if this lever has been pulled
    private LeverManager leverManager; // Reference to the manager

    public PullLever() : base() { }

    void Start()
    {
        leverManager = FindObjectOfType<LeverManager>();
    }
    void Update()
    {
        if (!leverPulled && Input.GetMouseButtonTriggered(0))
        {
            string hitObjectTag = playerInteraction.RayHitString; // Object the player clicked on
            string thisLeverTag = InternalCalls.GetTag(mEntityID); // This lever's tag

            if (hitObjectTag == thisLeverTag) // Check if player clicked this lever
            {
                ActivateLever();
            }
        }
    }

    private void ActivateLever()
    {
        InternalCalls.PlaySound(mEntityID, "IncoherentWhispers");
        InternalCalls.PlayAnimation(mEntityID, "SwitchOff");
        leverPulled = true; // Prevent future interaction

        if (leverManager != null)
        {
            leverManager.LeverPulled(); // Notify the manager
        }
    }
}

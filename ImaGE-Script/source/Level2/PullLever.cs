using IGE.Utils;
using System;
using System.Runtime.CompilerServices;

public class PullLever : Entity
{
    public PlayerInteraction playerInteraction;
    private bool leverPulled = false; // Track if this lever has been pulled

    public PullLever() : base() { }

    void Update()
    {
        if (!leverPulled && Input.GetMouseButtonTriggered(0))
        {
            string hitObjectTag = playerInteraction.RayHitString; // Object the player clicked on
            string thisLeverTag = InternalCalls.GetTag(mEntityID); // This lever's tag

            if (hitObjectTag == thisLeverTag) // Check if player clicked this lever
            {
                Bitch();
            }
        }
    }

    private void Bitch()
    {
        InternalCalls.PlaySound(mEntityID, "IncoherentWhispers");
        InternalCalls.PlayAnimation(mEntityID, "SwitchOn");
        leverPulled = true; // Prevent future interaction
    }
}

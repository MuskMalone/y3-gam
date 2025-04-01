using IGE.Utils;
using System;
using System.Runtime.CompilerServices;

public class LeverPuzzle : Entity
{
    public PlayerInteraction playerInteraction;
    public Entity[] levers; // Array of levers
    private bool[] leverPulled; // Tracks if a lever has been pulled

    public LeverPuzzle() : base() { }

    void Start()
    {
        // Initialize the array to track lever states
        leverPulled = new bool[levers.Length];
    }

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        string hitObjectTag = playerInteraction.RayHitString; // The tag of the clicked object

        if (mouseClicked)
        {
            for (int i = 0; i < levers.Length; i++)
            {
                string leverTag = InternalCalls.GetTag(levers[i].mEntityID);

                if (!leverPulled[i] && hitObjectTag == leverTag) // Check if this lever is hit
                {
                    PullLever(i);
                    break;
                }
            }
        }
    }

    private void PullLever(int index)
    {
        InternalCalls.PlaySound(mEntityID, "IncoherentWhispers");
        levers[index].GetComponent<Animation>().Play("SwitchOn");
        leverPulled[index] = true; // Prevent future interaction
    }
}

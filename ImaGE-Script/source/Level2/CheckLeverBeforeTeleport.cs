using IGE.Utils;
using System;

public class CheckLeverBeforeTeleport : Entity
{
    // Reference to the player interaction so we can see what the player is clicking on.
    public PlayerInteraction playerInteraction;

    // Optionally, you can assign Lever1 from the editor if needed
    public Entity Lever1;

    // This flag will be set once Lever1 is hit, and then remain true.
    private bool lever1HitBefore = false;

    void Update()
    {
        // Check if the player clicks (or triggers the mouse button).
        if (Input.GetMouseButtonTriggered(0))
        {
            // Get the tag of the object that was hit by the player's raycast.
            string hitObjectTag = playerInteraction.RayHitString;

            // Check if the hit object is Lever1 and if it hasn't been hit before.
            if (hitObjectTag == "Lever2" && !lever1HitBefore)
            {
                lever1HitBefore = true;
                Debug.Log("Lever1 has been hit!");
                // You might also trigger additional behavior here if needed.
            }
        }
    }

    // A public accessor so other scripts (like Hex Teleport) can check the status.
    public bool IsLever1Hit()
    {
        return lever1HitBefore;
    }
}

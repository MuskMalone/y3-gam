using IGE.Utils;
using System;
using System.Collections.Generic;

public class LeverManager : Entity
{
    public Entity playerCamera;
    public Entity tableCamera;
    public PlayerMove playerMove;
    //public Entity door; // The door to unlock
    private int leversPulled = 0;
    private int totalLevers = 5; // Set total number of levers required
    private bool isCameraSwitched = false;  // Track if we're in table view
    private float switchBackTime = 0f;      // Store when to switch back
    private float switchDuration = 4.5f;

    public LeverManager() : base() { }

    void Start()
    {
        Console.WriteLine("Lever Manager Initialized. Waiting for levers to be pulled.");
    }


    //public void LeverPulled()
    //{
    //    leversPulled++;
    //    Console.WriteLine($"Lever pulled! {leversPulled}/{totalLevers} levers activated.");
    //    if (playerMove != null)
    //    {
    //        playerMove.FreezePlayer(); // Freeze player
    //    }


    //    SetTableCameraAsMain(); // Instantly switch camera

    //    ActivateLeverLight(leversPulled); // Activate the correct lever light

    //    SetPlayerCameraAsMain(); // Instantly switch back

    //    if (playerMove != null)
    //    {
    //        playerMove.UnfreezePlayer(); // Unfreeze player
    //    }
    //}

    void Update()
    {
        // Check if we're in table view and if the switch-back time has been reached
        if (isCameraSwitched && Time.gameTime >= switchBackTime)
        {
            SetPlayerCameraAsMain();  // Switch back to player view
            isCameraSwitched = false; // Reset flag

            if (playerMove != null)
            {
                playerMove.UnfreezePlayer(); // Allow player to move again
            }
        }
    }

    public void LeverPulled()
    {
        leversPulled++;
        Console.WriteLine($"Lever pulled! {leversPulled}/{totalLevers} levers activated.");

        if (playerMove != null)
        {
            playerMove.FreezePlayer(); // Freeze player movement
        }

        SetTableCameraAsMain(); // Switch to table camera
        isCameraSwitched = true; // Set flag
        switchBackTime = Time.gameTime + switchDuration; // Set when to switch back
    }

    private void SetPlayerCameraAsMain()
    {
        Console.WriteLine("Entered SetCamera as Main");
        InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
        InternalCalls.SetTag(tableCamera.mEntityID, "hexTableCamera");
    }

    private void SetTableCameraAsMain()
    {
        InternalCalls.SetTag(playerCamera.mEntityID, "hexTableCamera");
        InternalCalls.SetTag(tableCamera.mEntityID, "MainCamera");
    }

    private void UnlockFrag()
    {
        Console.WriteLine("All levers pulled! The door unlocks...");
    }
}

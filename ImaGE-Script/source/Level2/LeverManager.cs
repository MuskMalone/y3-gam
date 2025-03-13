using IGE.Utils;
using System;
using System.Collections.Generic;

public class LeverManager : Entity
{
    public Entity leverLight1;
    public Entity leverLight2;
    public Entity leverLight3;
    public Entity leverLight4;
    public Entity leverLight5;

    public Entity leverBallGray1, leverBallGold1;
    public Entity leverBallGray2, leverBallGold2;
    public Entity leverBallGray3, leverBallGold3;
    public Entity leverBallGray4, leverBallGold4;
    public Entity leverBallGray5, leverBallGold5;

    public Entity playerCamera;
    public Entity tableCamera;
    public PlayerMove playerMove;
    //public Entity door; // The door to unlock
    private int leversPulled = 0;
    private int totalLevers = 5; // Set total number of levers required
    private float cameraSwitchTime = 0f;
    private float switchDuration = 4.0f;

    public LeverManager() : base() { }

    void Start()
    {
        Console.WriteLine("Lever Manager Initialized. Waiting for levers to be pulled.");

        InternalCalls.SetLightIntensity(leverLight1.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight2.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight3.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight4.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight5.mEntityID, 0.0f);

        leverBallGray1.SetActive(true);
        leverBallGray2.SetActive(true); leverBallGray3.SetActive(true);
        leverBallGray4.SetActive(true); leverBallGray5.SetActive(true);
        leverBallGold1.SetActive(false);
        leverBallGold2.SetActive(false); leverBallGold3.SetActive(false); leverBallGold4.SetActive(false);
        leverBallGold5.SetActive(false);
    }


    public void LeverPulled()
    {
        leversPulled++;
        Console.WriteLine($"Lever pulled! {leversPulled}/{totalLevers} levers activated.");
        if (playerMove != null)
        {
            playerMove.FreezePlayer(); // Freeze player
        }


        SetTableCameraAsMain(); // Instantly switch camera

        ActivateLeverLight(leversPulled); // Activate the correct lever light

        SetPlayerCameraAsMain(); // Instantly switch back

        if (playerMove != null)
        {
            playerMove.UnfreezePlayer(); // Unfreeze player
        }
    }

    private void ActivateLeverLight(int leverIndex)
    {
        switch (leverIndex)
        {
            case 1:
                InternalCalls.SetLightIntensity(leverLight1.mEntityID, 5.0f);
                leverBallGray1.SetActive(false);
                leverBallGold1.SetActive(true);
                break;
            case 2:
                InternalCalls.SetLightIntensity(leverLight2.mEntityID, 5.0f);
                leverBallGray2.SetActive(false);
                leverBallGold2.SetActive(true);
                break;
            case 3:
                InternalCalls.SetLightIntensity(leverLight3.mEntityID, 5.0f);
                leverBallGray3.SetActive(false);
                leverBallGold3.SetActive(true);
                break;
            case 4:
                InternalCalls.SetLightIntensity(leverLight4.mEntityID, 5.0f);
                leverBallGray4.SetActive(false);
                leverBallGold4.SetActive(true);
                break;
            case 5:
                InternalCalls.SetLightIntensity(leverLight5.mEntityID, 5.0f);
                leverBallGray5.SetActive(false);
                leverBallGold5.SetActive(true);
                UnlockFrag();
                break;
        }
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

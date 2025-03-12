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
    //public Entity door; // The door to unlock
    private int leversPulled = 0;
    private int totalLevers = 5; // Set total number of levers required

    public LeverManager() : base() { }

    void Start()
    {
        Console.WriteLine("Lever Manager Initialized. Waiting for levers to be pulled.");

        InternalCalls.SetLightIntensity(leverLight1.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight2.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight3.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight4.mEntityID, 0.0f);
        InternalCalls.SetLightIntensity(leverLight5.mEntityID, 0.0f);
    }

    public void LeverPulled()
    {
        leversPulled++;
        Console.WriteLine($"Lever pulled! {leversPulled}/{totalLevers} levers activated.");

        // Light up a corresponding light
        switch (leversPulled)
        {
            case 1:
                InternalCalls.SetLightIntensity(leverLight1.mEntityID, 5.0f);
                Console.WriteLine("Light 1 turned on!");
                break;
            case 2:
                InternalCalls.SetLightIntensity(leverLight2.mEntityID, 5.0f);
                Console.WriteLine("Light 2 turned on!");
                break;
            case 3:
                InternalCalls.SetLightIntensity(leverLight3.mEntityID, 5.0f);
                Console.WriteLine("Light 3 turned on!");
                break;
            case 4:
                InternalCalls.SetLightIntensity(leverLight4.mEntityID, 5.0f);
                Console.WriteLine("Light 4 turned on!");
                break;
            case 5:
                InternalCalls.SetLightIntensity(leverLight5.mEntityID, 5.0f);
                Console.WriteLine("Light 5 turned on!");
                UnlockDoor();
                break;
        }
    }

    private void UnlockDoor()
    {
        Console.WriteLine("All levers pulled! The door unlocks...");
        //InternalCalls.PlaySound(door.mEntityID, "UnlockDoorSound");
        //InternalCalls.PlayAnimation(door.mEntityID, "DoorOpen");
    }
}

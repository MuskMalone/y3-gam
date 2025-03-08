using System;
using IGE.Utils;
using System.Numerics;

public class BGMLevel1 : Entity
{
    public Entity player;
    public Entity doorEntryTrigger;
    public Entity unlockDoorUI;
    public float distanceFromPlayerFromDoor;
    public bool isEnteredHallway = false;

    void Start() { }

    void Update()
    {
        if (!unlockDoorUI.IsActive())
        {
            Vector3 playerLoc = InternalCalls.GetWorldPosition(player.mEntityID);
            Vector3 doorLoc = InternalCalls.GetWorldPosition(doorEntryTrigger.mEntityID);

            distanceFromPlayerFromDoor = (doorLoc - playerLoc).Length();

            isEnteredHallway = playerLoc.X > doorLoc.X;

            if (isEnteredHallway)
            {

            }
            else
            {

            }
        }
    }
}

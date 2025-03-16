using IGE.Utils;
using System;
using System.Numerics;

public class HexTeleport : Entity
{
    public PlayerMove playerMove;

    public Vector3 teleportPosition7 = new Vector3(59.034f, 120.296f, -511.751f); //7
    public Vector3 teleportPosition2 = new Vector3(5.777f, 91.919f, -481.967f); //2
    public Vector3 teleportPosition6 = new Vector3(9.061f, 145.719f, -379.98f); //6
    public Vector3 teleportPosition3 = new Vector3(23.755f, 148.257f, -500.434f); //3
    public Vector3 teleportPosition4 = new Vector3(-2.203f, 176.774f, -448.731f); //4
    public Vector3 teleportPosition5 = new Vector3(140.922f, 196.080f, -442.446f); //5
    public Vector3 teleportPosition1 = new Vector3(116.756f, 123.739f, -366.274f); //1



    void Update()
    {
        //if (Input.GetKeyDown(KeyCode.B)) TeleportPlayer(teleportPosition7); //7
        //else if (Input.GetKeyDown(KeyCode.N)) TeleportPlayer(teleportPosition2); //2
        //else if (Input.GetKeyDown(KeyCode.M)) TeleportPlayer(teleportPosition6); //6 
        //else if (Input.GetKeyDown(KeyCode.APOSTROPHE)) TeleportPlayer(teleportPosition3); //3
        //else if (Input.GetKeyDown(KeyCode.H)) TeleportPlayer(teleportPosition4);//4
        //else if (Input.GetKeyDown(KeyCode.BACKSLASH)) TeleportPlayer(teleportPosition5); //5
        //else if (Input.GetKeyDown(KeyCode.EQUAL)) TeleportPlayer(teleportPosition1); //1
        //else if (Input.GetKeyTriggered(KeyCode.MINUS)) TeleportPlayer(InternalCalls.GetPosition(playerMove.mEntityID) - new Vector3(0f, -5f, 0.5f));
    }

    public void TeleportPlayer(Vector3 newPosition)
    {
        playerMove.FreezePlayer(); // Temporarily freeze player to ensure teleportation is applied
        InternalCalls.SetPosition(playerMove.mEntityID, ref newPosition); // Set the player's new position
        InternalCalls.UpdatePhysicsToTransform(playerMove.mEntityID); // Ensure physics updates after teleporting
        playerMove.UnfreezePlayer(); // Re-enable player movement

        Debug.Log($"Teleported player to: {newPosition}");
    }
}

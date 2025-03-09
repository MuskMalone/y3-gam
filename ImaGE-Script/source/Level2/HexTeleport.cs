using IGE.Utils;
using System;
using System.Numerics;

public class HexTeleport : Entity
{
    public PlayerMove playerMove; 
   
    //public Vector3 teleportPosition0 = new Vector3(0f, 0f, 0f); 
    //public Vector3 teleportPosition9 = new Vector3(10f, 0f, 10f);
    //public Vector3 teleportPosition8 = new Vector3(-10f, 0f, -10f);

    private Vector3 teleportPosition0 = new Vector3(111.726f, 117.319f, 369.132f);
    private Vector3 teleportPosition9 = new Vector3(5.777f, 91.919f, -481.967f);
    private Vector3 teleportPosition8 = new Vector3(9.061f, 145.719f, -379.98f);

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            TeleportPlayer(teleportPosition0);
        }
        else if (Input.GetKeyDown(KeyCode.N))
        {
            TeleportPlayer(teleportPosition9);
        }
        else if (Input.GetKeyDown(KeyCode.M))
        {
            TeleportPlayer(teleportPosition8);
        }
    }

    private void TeleportPlayer(Vector3 newPosition)
    {
        InternalCalls.SetPosition(playerMove.mEntityID, ref newPosition); 
        //InternalCalls.PlaySound(mEntityID, "TeleportComplete"); 
        Debug.Log($"Teleported player to: {newPosition}");
    }
}

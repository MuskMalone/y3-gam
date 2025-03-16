using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Numerics;

public class LeverManager : Entity
{
  public Entity playerCamera;
  public Entity tableCamera;
  public Entity fragmentGlass;
  public PlayerMove playerMove;
  //public Entity door; // The door to unlock

  public float timeBeforeOrbsDrop;  // how much time to wait after all levers pulled

  private int leversPulled = 0;
  private int totalLevers = 5; // Set total number of levers required
  private float switchBackTime = 0f;      // Store when to switch back
  private float switchDuration = 4.5f;
  private List<HexTableOrb> orbs = new List<HexTableOrb>();

    private Vector3 teleportPositionTable = new Vector3(64.307f, 60.942f, -390.26f);
    private HexTeleport hexTeleport;

    private enum State
  {
    IDLE,
    TABLE_CAM,
    DEACTIVATE_ORBS
  }
  private State currState = State.IDLE;

  public LeverManager() : base() { }

  void Start()
  {
    Console.WriteLine("Lever Manager Initialized. Waiting for levers to be pulled.");
        hexTeleport = FindObjectOfType<HexTeleport>();
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
    switch (currState)
    {
      case State.IDLE:

        break;

      case State.TABLE_CAM:
        {
          Console.WriteLine(Time.gameTime.ToString());
          // Check if we're in table view and if the switch-back time has been reached
          if (Time.gameTime >= switchBackTime)
          {
            SetPlayerCameraAsMain();  // Switch back to player view
            currState = State.IDLE;

            if (playerMove != null)
            {
              playerMove.UnfreezePlayer(); // Allow player to move again
            }

            // if all levers pulled, transition to next phase
            if (leversPulled >= totalLevers)
            {
              switchBackTime = Time.gameTime + timeBeforeOrbsDrop;  // just gonna reuse switchBackTime
                            hexTeleport.TeleportPlayer(teleportPositionTable);
              currState = State.DEACTIVATE_ORBS;
            }
          }

          break;
        }

      case State.DEACTIVATE_ORBS:
        {
          if (Time.gameTime < switchBackTime) { return; }

          // trigger the next sequence for the orbs
          foreach (HexTableOrb orb in orbs)
          {
            orb.LosePower();
          }
          orbs = null;  // we dont need the list anymore

          // not sure if theres anything else to do after this
          currState = State.IDLE;
          break;
        }
    }
  }

  public void AddOrb(HexTableOrb orb) { orbs.Add(orb); }

  public void LeverPulled()
  {
    ++leversPulled;
    Console.WriteLine($"Lever pulled! {leversPulled}/{totalLevers} levers activated.");

    if (playerMove != null)
    {
      playerMove.FreezePlayer(); // Freeze player movement
    }

    SetTableCameraAsMain(); // Switch to table camera
    currState = State.TABLE_CAM;
    switchBackTime = Time.gameTime + switchDuration; // Set when to switch back
  }

  public void OrbShattered()
  {
    // if all orbs shattered, unlock fragment
    if (--leversPulled <= 0)
    {
      fragmentGlass.SetActive(false);
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

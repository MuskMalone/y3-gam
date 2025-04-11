using IGE.Utils;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Numerics;

public class LeverManager : Entity
{
  public Entity SFX;
  public Entity playerCamera;
  public Entity tableCamera;
  public Entity fragmentGlass;
  public PlayerMove playerMove;
  public BlackBorder blackBorders;

  public float timeBeforeOrbsDrop;  // how much time to wait after orbs get violent
  public float timeBeforeFinalTeleport; // delay after all levers pulled

  private int leversPulled = 0;
  private int totalLevers = 5; // Set total number of levers required
  private float switchDuration = 4.5f;
  private float timeElapsed = 0f;
  private List<HexTableOrb> orbs = new List<HexTableOrb>();

  private Vector3 teleportPositionTable = new Vector3(64.307f, 60.942f, -390.26f);
  private HexTeleport hexTeleport;

  private enum State
  {
    IDLE,
    TABLE_CAM,
    TELEPORT_PLAYER,
    DEACTIVATE_ORBS
  }
  private State currState = State.IDLE;

  public LeverManager() : base() { }

  void Start()
  {
  //Console.WriteLine("Lever Manager Initialized. Waiting for levers to be pulled.");
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
    //Debug.Log(currState.ToString());
    switch (currState)
    {
      case State.IDLE:
        if (Input.GetKeyTriggered(KeyCode.V))
        {
          playerMove.SetRotation(new Vector3(-1f, 1.9f, 0));  // make player look at table
        }
        break;

      case State.TABLE_CAM:
        {
          //Debug.Log(timeElapsed.ToString() + " / " + switchDuration.ToString());

          timeElapsed += Time.deltaTime;

          // Check if we're in table view and if the switch-back time has been reached
          if (timeElapsed >= switchDuration)
          {
            timeElapsed = 0f;
            blackBorders.HideBlackBorders();
            SetPlayerCameraAsMain();  // Switch back to player view
            currState = State.IDLE;

            if (playerMove != null)
            {
              playerMove.UnfreezePlayer(); // Allow player to move again
            }

            // if all levers pulled, transition to next phase
            if (leversPulled >= totalLevers)
            {
              InternalCalls.PlaySound(SFX.mEntityID, "..\\Assets\\Audio\\Earthquake_Rumble1_SFX.wav");
              currState = State.TELEPORT_PLAYER;
            }
          }

          break;
        }

      case State.TELEPORT_PLAYER:
        {
          timeElapsed += Time.deltaTime;
          if (timeElapsed >= timeBeforeFinalTeleport)
          {
            hexTeleport.TeleportPlayer(teleportPositionTable);
            playerMove.SetRotation(new Vector3(-1f, 1.9f, 0));  // make player look at table
            currState = State.DEACTIVATE_ORBS;
          }

          break;
        }

      case State.DEACTIVATE_ORBS:
        {
          timeElapsed += Time.deltaTime;
          InternalCalls.SetSoundVolume(SFX.mEntityID, "..\\Assets\\Audio\\Earthquake_Rumble1_SFX.wav", Easing.Linear(0.3f, 0.7f, timeElapsed / (timeBeforeOrbsDrop)));
          if (timeElapsed < timeBeforeOrbsDrop) { return; }

          timeElapsed = 0f;
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
    //Console.WriteLine($"Lever pulled! {leversPulled}/{totalLevers} levers activated.");

    if (playerMove != null)
    {
      playerMove.FreezePlayer(); // Freeze player movement
    }

    blackBorders.DisplayBlackBorders();
    SetTableCameraAsMain(); // Switch to table camera
    currState = State.TABLE_CAM;
    //timeElapsed = 0f;
    //Debug.Log("STATE CHANGED TO " + currState.ToString());
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
    //Console.WriteLine("Entered SetCamera as Main");
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(tableCamera.mEntityID, "hexTableCamera");
  }

  private void SetTableCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "hexTableCamera");
    InternalCalls.SetTag(tableCamera.mEntityID, "MainCamera");
  }
}

﻿using IGE.Utils;
using System;
using System.Collections.Generic;

public class LeverManager : Entity
{
  public Entity playerCamera;
  public Entity tableCamera;
  public PlayerMove playerMove;
  public Entity holderCollider1, holderCollider2, 
    holderCollider3, holderCollider4, holderCollider5;
  //public Entity door; // The door to unlock


  public float timeBeforeOrbsDrop;  // how much time to wait after all levers pulled

  private int leversPulled = 0;
  private int totalLevers = 5; // Set total number of levers required
  private float switchBackTime = 0f;      // Store when to switch back
  private float switchDuration = 4.5f;
  private List<HexTableOrb> orbs = new List<HexTableOrb>();

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
    SetHolderCollidersActive(false);
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
    switch (currState)
    {
      case State.IDLE:

        break;

      case State.TABLE_CAM:
        {
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

          SetHolderCollidersActive(true);

          // not sure if theres anything else to do after this
          currState = State.IDLE;
          break;
        }
    }
  }

  public void AddOrb(ref HexTableOrb orb) { orbs.Add(orb); }

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


  private void SetHolderCollidersActive(bool active)
  {
    holderCollider1.SetActive(active); holderCollider2.SetActive(active);
    holderCollider3.SetActive(active); holderCollider4.SetActive(active);
    holderCollider5.SetActive(active);
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

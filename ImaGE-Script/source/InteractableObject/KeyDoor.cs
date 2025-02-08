using IGE.Utils;
using System;
using System.Numerics;

public class KeyDoor : Entity
{
  // Script to be placed in the Puzzle_KeyDoor Entity (Parent)
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity unlockDoorUI;
  public string[] lockedDialogue;
  public Dialogue dialogueSystem;
  public bool doorInteraction = true;
  public string doorAnimName;  // input from inspector based on name in anim component

  private bool doorFlag = false;
  private string currentAnim = null;

  void Start()
  {
    unlockDoorUI?.SetActive(false);
  }

  void Update()
  {
    if (doorInteraction)
    {
      bool isDoorHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetMouseButtonTriggered(0) && isDoorHit && !doorFlag)
      {
        if (!inventoryScript.keyEquipped && !dialogueSystem.isInDialogueMode)
        {
          InternalCalls.PlaySound(mEntityID, "LockedDoor");
          dialogueSystem.SetDialogue(lockedDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Sad });
          doorFlag = true;
          return;
        }

        doorInteraction = false;
      }
      unlockDoorUI.SetActive(isDoorHit);

      if (!isDoorHit)
      {
        doorFlag = false;
      }
    }
    // if an animation is in progress
    else if (!string.IsNullOrEmpty(currentAnim))
    {
      if (currentAnim == doorAnimName)
      {
        uint parent = InternalCalls.GetParentByID(mEntityID);

        // align the collider to the animated transform
        InternalCalls.UpdatePhysicsToTransform(mEntityID);

        // end of animation sequence, clear the current anim
        if (!InternalCalls.IsPlayingAnimation(parent)) { currentAnim = null; }
      }
    }
  }

  public void UnlockDoor()
  {
    currentAnim = doorAnimName;
    InternalCalls.PlaySound(mEntityID, "UnlockDoor");
    InternalCalls.PlayAnimation(InternalCalls.GetParentByID(mEntityID), doorAnimName);
    unlockDoorUI.SetActive(false);
  }
}

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
  public string doorSlamAnimName;
  public BlackBorder blackBorder;
  public Entity transitionCorridorTrigger;

  public Entity playerCamera;
  public Entity keyCamera;
  float elapsedTime = 0.0f;
  float zoomOutDuration = 2.50f;
  bool initialAnimation = true;
  private Vector3 zoomInPos;
  private Vector3 zoomOutPos;
  bool isZoomingOut = true;

  public Entity corridorTrigger;
  public Entity motherHumming;
  public bool triggerInteraction = true;

  private string currentAnim = null;

    private bool isIntroDialogue1Active = false;
    private bool hasPlayedLine0Sound = false;

    void Start()
  {
    unlockDoorUI?.SetActive(false);
  }

  void Update()
  {

        if (isIntroDialogue1Active)
        {
            if(dialogueSystem.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L1_DOOR");
                hasPlayedLine0Sound = true;
                isIntroDialogue1Active = false;
            }
        }

        if (!dialogueSystem.isInDialogueMode)
        {
            hasPlayedLine0Sound = false;
            //hasPlayedLine1Sound = false;
        }
        if (doorInteraction)
    {
      bool isDoorHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (isDoorHit && Input.GetMouseButtonTriggered(0))
      {
        if (inventoryScript.keyEquipped)
        {
          doorInteraction = false;
        }
        else if (!dialogueSystem.isInDialogueMode)
        {
          InternalCalls.PlaySound(mEntityID, "LockedDoor");
          dialogueSystem.SetDialogue(lockedDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Sad });
                    isIntroDialogue1Active = true;
          return;
        }
      }
      else
      {
        unlockDoorUI.SetActive(isDoorHit);
      }
    }
    else if (!doorInteraction && triggerInteraction)
    {
      if (InternalCalls.OnTriggerEnter(corridorTrigger.mEntityID, blackBorder.playerMove.mEntityID))
      {
        SlamDoor();
        triggerInteraction = false;
      }
    }

    // if an animation is in progress
    if (!string.IsNullOrEmpty(currentAnim))
    {
      uint parent = InternalCalls.GetParentByID(mEntityID);

      // align the collider to the animated transform
      InternalCalls.UpdatePhysicsToTransform(mEntityID);

      if (currentAnim == doorSlamAnimName)
      {
        if (initialAnimation)
        {
          InternalCalls.PlaySound(mEntityID, "DoorSwing");
          initialAnimation = false;
        }
        else if (!InternalCalls.IsPlayingAnimation(parent))
        {
          InternalCalls.PlaySound(mEntityID, "DoorSlam");
          currentAnim = null;
          initialAnimation = true;
        }
      }

      if (currentAnim == doorAnimName)
      {
        if (initialAnimation)
        {
          zoomInPos = InternalCalls.GetPosition(keyCamera.mEntityID);
          zoomOutPos = zoomInPos + new Vector3(-15, 0, 0);
          initialAnimation = false;
          InternalCalls.PlaySound(mEntityID, "DoorSwing");
        }

        if (isZoomingOut)
        {
          elapsedTime += Time.deltaTime;
          float t = Mathf.SmoothStep(elapsedTime / zoomOutDuration);
          Vector3 newPos = Vector3.Lerp(zoomInPos, zoomOutPos, t);
          InternalCalls.SetPosition(keyCamera.mEntityID, ref newPos);

          if (elapsedTime >= zoomOutDuration)
          {
            elapsedTime = 0.0f;
            isZoomingOut = false;
          }
        }

        // end of animation sequence, clear the current anim
        if (!InternalCalls.IsPlayingAnimation(parent)) {
          currentAnim = null;
          initialAnimation = true;
          SetPlayerCameraAsMain();
          blackBorder.HideBlackBorders();
        }
      }
    }
  }

  public void UnlockDoor()
  {
    currentAnim = doorAnimName;
    InternalCalls.PlaySound(mEntityID, "UnlockDoor");
    InternalCalls.PlayAnimation(InternalCalls.GetParentByID(mEntityID), doorAnimName);
    unlockDoorUI.SetActive(false);
    InternalCalls.PlaySound(motherHumming.mEntityID, "MotherHumming");
  }

  public void SlamDoor()
  {
    currentAnim = doorSlamAnimName;
    transitionCorridorTrigger?.FindScript<TransitionHallwayTrigger>().PlayerTrapped();
    InternalCalls.PlayAnimation(InternalCalls.GetParentByID(mEntityID), doorSlamAnimName);
  }

  private void SetPlayerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(keyCamera.mEntityID, "KeyCamera");
  }
}

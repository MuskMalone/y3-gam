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

  public PlayerMove playerMove;
  public Entity playerCamera;
  public Entity keyCamera;
  float elapsedTime = 0.0f;
  float zoomOutDuration = 2.50f;
  bool initialAnimation = true;
  private Vector3 zoomInPos;
  private Vector3 zoomOutPos;
  bool isZoomingOut = true;

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

        if (initialAnimation)
        {
          Console.WriteLine("Start");
          Debug.Log("Start");
          zoomInPos = InternalCalls.GetPosition(keyCamera.mEntityID);
          zoomOutPos = zoomInPos + new Vector3(-15, 0, 0);
          initialAnimation = false;
          InternalCalls.PlaySound(mEntityID, "DoorSwing");
        }

        if (isZoomingOut)
        {
          Console.WriteLine("Zooming");
          Debug.Log("Zooming");
          elapsedTime += Time.deltaTime;
          float t = elapsedTime / zoomOutDuration;
          t = t * t * (3 - 2 * t); // SmoothStep easing
          Vector3 newPos = Vector3.Lerp(zoomInPos, zoomOutPos, t);
          InternalCalls.SetPosition(keyCamera.mEntityID, ref newPos);

          if (elapsedTime >= zoomOutDuration)
          {
            Console.WriteLine("Zooming End");
            elapsedTime = 0.0f;
            isZoomingOut = false;
          }
        }

        // end of animation sequence, clear the current anim
        if (!InternalCalls.IsPlayingAnimation(parent)) {
          Console.WriteLine("End");
          Debug.Log("End");
          currentAnim = null;
          SetPlayerCameraAsMain();
          playerMove.UnfreezePlayer();
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
  }

  private void SetPlayerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(keyCamera.mEntityID, "KeyCamera");
  }
}

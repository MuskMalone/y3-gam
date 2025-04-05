using IGE.Utils;
using System;
using System.Numerics;
using System.Text;
using static System.TimeZoneInfo;

public class TheTwinDoors : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity level2Dialogue;
  public string[] lockedDoorDialogue;
  public Entity leftDoor, rightDoor;
  public Entity interactDoorUI;
  public string doorAnimName;

    
    private bool hasPlayedLine0Sound = false;
    private bool isDialogueActive = false;

    private enum State
  {
    CLOSED,
    ANIMATION,
    OPEN
  }
  private State currState = State.CLOSED;
  private TutorialDialogue dialogueScript;

  public TheTwinDoors() : base() {}

  void Start()
  {
    dialogueScript = level2Dialogue.FindScript<TutorialDialogue>();
  }

  void Update()
  {

        if (isDialogueActive)
        {
            if (dialogueScript.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L2_4_VER2");
                hasPlayedLine0Sound = true;
                isDialogueActive = false;
            }
            
        }

        if (!dialogueScript.isInDialogueMode)
        {
            hasPlayedLine0Sound = false;
            
        }
        switch (currState)
    {
      case State.CLOSED:
        {
          // both doors are prefixed with "TwinDoors "
          bool doorHit = playerInteraction.RayHitString.StartsWith("TwinDoors ");
          if (Input.GetKeyTriggered(KeyCode.MOUSE_BUTTON_1) && doorHit && !dialogueScript.isInDialogueMode)
          {
            InternalCalls.PlaySound(mEntityID, "LockedDoor");
            dialogueScript.SetDialogue(lockedDoorDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Thinking });
                        isDialogueActive = true;
            interactDoorUI.SetActive(false);
          }
          else
          {
            interactDoorUI.SetActive(doorHit);
          }

          break;
        }
      case State.ANIMATION:
        {
          // align collider during anim
          if (leftDoor.GetComponent<Animation>().IsPlaying())
          {
            InternalCalls.UpdatePhysicsToTransform(leftDoor.mEntityID);
            InternalCalls.UpdatePhysicsToTransform(rightDoor.mEntityID);
          }
          else
          {
            currState = State.OPEN;
          }

          break;
        }
      case State.OPEN:
      {
        Destroy(this);  // no longer need this script
        break;
      }
    }
  }

  public void UnlockDoors()
  {
    leftDoor.GetComponent<Animation>().Play(doorAnimName);
    rightDoor.GetComponent<Animation>().Play(doorAnimName);
    currState = State.ANIMATION;
    interactDoorUI.SetActive(false);
  }
}
using IGE.Utils;
using System.Numerics;

public class KeyDoor : Entity
{
  // Script to be placed in the Puzzle_KeyDoor Entity (Parent)
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity unlockDoorUI;
  public string[] lockedDialogue;
  public Dialogue dialogueSystem;
  private bool doorFlag = false;
  public bool doorInteraction = true;

  private Vector3 UnlockedPosition = new Vector3(57.197f, 13.759f, 52.316f);
  private Quaternion UnlockedQuaternion = new Quaternion(0f, -0.2745087f, 0f, 0.9615892f);

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

        UnlockDoor();
        return;
      }
      unlockDoorUI.SetActive(isDoorHit);

      if (!isDoorHit)
      {
        doorFlag = false;
      }
    }
  }

  private void UnlockDoor()
  {
    InternalCalls.PlaySound(mEntityID, "UnlockDoor");
    InternalCalls.SetPosition(mEntityID, ref UnlockedPosition);
    InternalCalls.SetRotation(mEntityID, ref UnlockedQuaternion);
    InternalCalls.SetWorldRotation(mEntityID, ref UnlockedQuaternion);
    unlockDoorUI.SetActive(false);
    SetActive(false);
    doorInteraction = false;
  }
}

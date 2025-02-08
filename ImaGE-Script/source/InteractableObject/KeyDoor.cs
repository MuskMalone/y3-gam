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
  public Entity unlockedColliderEntity;

  private Vector3 UnlockedPosition = new Vector3(58.293f, 14.157f, 57.587f);
  private Vector3 UnlockedEuler = new Vector3(0.089f, 89.778f, 0.088f);

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
    InternalCalls.PlayAnimation(InternalCalls.GetParentByID(mEntityID), "OpenDoor");
    //InternalCalls.SetWorldPosition(mEntityID, ref UnlockedPosition);
    //InternalCalls.SetRotationEuler(mEntityID, ref UnlockedEuler);
    unlockDoorUI.SetActive(false);
    SetActive(false);
    unlockedColliderEntity.SetActive(true);
    doorInteraction = false;
  }
}

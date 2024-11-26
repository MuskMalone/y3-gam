using IGE.Utils;

public class KeyDoor : Entity
{
  // Script to be placed in the Puzzle_KeyDoor Entity (Parent)
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity unlockDoorUI;
  public string[] lockedDialogue;
  public Dialogue dialogueSystem;
  private bool doorFlag = false;

  void Start()
  {
    unlockDoorUI?.SetActive(false);
  }

  void Update()
  {
    bool isDoorHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetMouseButtonTriggered(0) && isDoorHit && !doorFlag)
    {
      if (!inventoryScript.keyEquipped)
      {
        InternalCalls.PlaySound(mEntityID, "LockedDoor");
        dialogueSystem.SetDialogue(lockedDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Sad });
        doorFlag = true;
      }
    }
    unlockDoorUI.SetActive(isDoorHit);

    if (!isDoorHit)
    {
      doorFlag = false;
    }
  }
}

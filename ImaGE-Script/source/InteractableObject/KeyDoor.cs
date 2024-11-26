using IGE.Utils;

public class KeyDoor : Entity
{
  // Script to be placed in the Puzzle_KeyDoor Entity (Parent)
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity unlockDoorUI;
  public string[] lockedDialogue;
  public Dialogue dialogueSystem;

  void Start()
  {
    unlockDoorUI?.SetActive(false);
  }

  void Update()
  {
    bool isDoorHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetMouseButtonTriggered(0) && isDoorHit)
    {
      if (!inventoryScript.keyEquipped)
      {
        InternalCalls.PlaySound(mEntityID, "LockedDoor");
        dialogueSystem.SetDialogue(lockedDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Sad });
      }
    }
    unlockDoorUI.SetActive(isDoorHit);
  }
}

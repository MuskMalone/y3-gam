using IGE.Utils;
using static Dialogue;

public class NightPainting : Entity, IInventoryItem
{
  public Entity _Image;
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Dialogue dialogueSystem;
  public string[] pitPaintingDialogue;

  private bool isBeingPickedUp = false;
  public Entity playerCamera;
  public PlayerMove playerMove;

  public string Name
  {
    get
    {
      return "NightPainting";
    }
  }

  public Entity Image
  {
    get
    {
      return _Image;
    }

    set
    {
      _Image = value;
    }
  }

  public void OnPickup()
  {
    SetActive(false);
  }

  public void OnUsed()
  {
    Destroy();
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    if (isBeingPickedUp)
    {
      if (Pickup.MoveAndShrink(this, playerInteraction.mEntityID, playerCamera.mEntityID))
      {
        InternalCalls.PlaySound(mEntityID, "PickupPainting");
        isBeingPickedUp = false;
        playerMove.UnfreezePlayer();
        inventoryScript.Additem(this);
        dialogueSystem.SetDialogue(pitPaintingDialogue, new Dialogue.Emotion[] { Emotion.Surprised, Emotion.Shocked });
      }
      return;
    }

    // For Painting Picking Up
    bool isPaintHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (isPaintHit && Input.GetKeyTriggered(KeyCode.E))
    {
      isBeingPickedUp = true;
      playerMove.FreezePlayer();
    }
    EToPickUpUI.SetActive(isPaintHit);
  }
}

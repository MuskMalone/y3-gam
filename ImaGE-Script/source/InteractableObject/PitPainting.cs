using IGE.Utils;
using static Dialogue;

public class PitPainting : Entity, IInventoryItem
{
  public Entity _Image;
  public Level3Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  //public Dialogue dialogueSystem;
  //public string[] pitPaintingDialogue;

  private bool isBeingPickedUp = false;
  public float finalDistanceAwayFromCamAfterPickup = 2f;
  public PlayerMove playerMove;
  public Entity playerCamera;

  public string Name
  {
    get
    {
      return "PitPainting";
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
        InternalCalls.PlaySound(mEntityID, "PickupObjects");
        inventoryScript.Additem(this);
        isBeingPickedUp = false;
        playerMove.UnfreezePlayer();
      }
      return;
    }

    // For Painting Picking Up
    bool isPaintHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (isPaintHit && Input.GetKeyTriggered(KeyCode.E))
    {
      isBeingPickedUp = true;
      playerMove.FreezePlayer();
      EToPickUpUI.SetActive(false);
      return;
      //dialogueSystem.SetDialogue(pitPaintingDialogue, new Dialogue.Emotion[] { Emotion.Surprised, Emotion.Shocked });
    }
    EToPickUpUI.SetActive(isPaintHit);
  }
}

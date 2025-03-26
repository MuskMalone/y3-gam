using IGE.Utils;

public class Crowbar : Entity, IInventoryItem
{
  // Script to be placed in the Crowbar Entity (Parent)
  public Entity _Image; 
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;

  private bool isBeingPickedUp = false;
  public Entity playerCamera;
  public PlayerMove playerMove;

  public string Name
  {
    get
    {
      return "Crowbar";
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
        isBeingPickedUp = false;
        inventoryScript.Additem(this);
        playerMove.UnfreezePlayer();
      }
      return;
    }

    bool isCrowbarHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetKeyTriggered(KeyCode.E) && isCrowbarHit)
    {
      isBeingPickedUp = true;
      playerMove.FreezePlayer();
      EToPickUpUI.SetActive(false);
      return;
    }
    EToPickUpUI.SetActive(isCrowbarHit);
  }
}

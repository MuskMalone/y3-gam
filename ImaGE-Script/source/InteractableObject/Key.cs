using IGE.Utils;

public class Key : Entity, IInventoryItem
{
  // Script to be placed in the Key Entity (Parent)
  public Entity _Image; // Selection UI
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public KeyDoor keyDoor;

  public string Name
  {
    get
    {
      return "Key";
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
    Destroy(mEntityID);
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    bool isKeyHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetKeyTriggered(KeyCode.E) && isKeyHit)
    {
      InternalCalls.PlaySound(mEntityID, "PickupObjects");
      inventoryScript.Additem(this);
    }
    EToPickUpUI.SetActive(isKeyHit);

    if (!keyDoor.doorInteraction)
    {
      inventoryScript.RemoveItem(this);
    }
  }
}

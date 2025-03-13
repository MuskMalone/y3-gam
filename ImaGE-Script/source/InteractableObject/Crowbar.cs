using IGE.Utils;

public class Crowbar : Entity, IInventoryItem
{
  // Script to be placed in the Crowbar Entity (Parent)
  public Entity _Image; 
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;

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
    bool isCrowbarHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetKeyTriggered(KeyCode.E) && isCrowbarHit)
    {
      InternalCalls.PlaySound(mEntityID, "PickupObjects");
      inventoryScript.Additem(this);
    }
    EToPickUpUI.SetActive(isCrowbarHit);
  }
}

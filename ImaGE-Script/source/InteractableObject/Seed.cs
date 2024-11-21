using IGE.Utils;

public class Seed : Entity, IInventoryItem
{
  public Entity _Image;
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;

  public string Name
  {
    get
    {
      return "Seed";
    }
  }

  public Entity Image
  {
    get
    {
      return _Image;
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
    bool isClicked = Input.GetKeyTriggered(KeyCode.E);
    bool isNoteHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);

    if (isClicked && isNoteHit)
    {
      inventoryScript.Additem(this);
    }

    EToPickUpUI.SetActive(isNoteHit);
  }
}

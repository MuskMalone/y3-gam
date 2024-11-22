using IGE.Utils;

public class Seed : Entity, IInventoryItem
{
  public Entity _Image;
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Entity EToPlantSeedUI;
  public Entity Pot;

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
    EToPlantSeedUI?.SetActive(false);
  }

  void Update()
  {
    // For Seed Picking Up
    bool isClicked = Input.GetKeyTriggered(KeyCode.E);
    bool isSeedHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    
    if (isClicked && isSeedHit)
    {
      inventoryScript.Additem(this);
    }
    EToPickUpUI.SetActive(isSeedHit);

    // For Seed Planting
    bool isPotHit = playerInteraction.RayHitString == InternalCalls.GetTag(Pot.mEntityID);
    if (inventoryScript.seedEquipped)
    {
      EToPlantSeedUI.SetActive(isPotHit);
    }
    else
    {
      EToPlantSeedUI.SetActive(false);
    }

    if (EToPlantSeedUI.IsActive() && isClicked)
    {
      inventoryScript.RemoveItem(this);
    }
  }
}

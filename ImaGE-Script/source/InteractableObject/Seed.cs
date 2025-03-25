using IGE.Utils;
using System;
using System.Numerics;

public class Seed : Entity, IInventoryItem
{
  public Entity _Image;
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Entity EToPlantSeedUI;
  public Entity Pot;
  public Entity Flower;
  public Entity playerCamera;
  private PictureAlign PictureAlignScript;

  private bool seedPlanted = false;
  private bool isBeingPickedUp = false;

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
    //Destroy(mEntityID);
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);
    EToPlantSeedUI?.SetActive(false);
    Flower?.SetActive(false);
    PictureAlignScript = FindObjectOfType<PictureAlign>();
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
      }
      return;
    }

    // For Seed Picking Up
    bool isSeedHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetKeyTriggered(KeyCode.E) && isSeedHit)
    {
      isBeingPickedUp = true;
    }
    EToPickUpUI.SetActive(isSeedHit);
    inventoryScript.pickupHandUI.SetActive(isSeedHit);

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

    if (EToPlantSeedUI.IsActive() && Input.GetMouseButtonTriggered(0))
    {
      InternalCalls.PlaySound(mEntityID, "PlantSeed");
      EToPlantSeedUI.SetActive(false);
      inventoryScript.RemoveItem(this);
      Pot.SetActive(true); // Sets the child seeds active as well
      seedPlanted = true;
    }
   
    if (PictureAlignScript.isNight && seedPlanted)
    {
      Flower.SetActive(true);
    }
  }
}

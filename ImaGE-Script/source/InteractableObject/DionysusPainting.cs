using System;
using IGE.Utils;
using static Dialogue;

public class DionysusPainting : Entity, IInventoryItem
{
    public Entity _Image;
    public Level2Inventory level2inventoryScript;
    public PlayerInteraction playerInteraction;
    public Entity EToPickUpUI;

  private bool isBeingPickedUp = false;
  public float finalDistanceAwayFromCamAfterPickup = 2f;
  public PlayerMove playerMove;
  public Entity playerCamera;

  public string Name
    {
        get
        {
            return "DionysusPainting";
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
      if (Pickup.MoveAndShrink(this, playerInteraction.mEntityID, playerCamera.mEntityID, finalDistanceAwayFromCamAfterPickup))
      {
        InternalCalls.PlaySound(mEntityID, "PickupPainting");
        level2inventoryScript.Additem(this);
        isBeingPickedUp = false;
        playerMove.UnfreezePlayer();
      }
      return;
    }

    // For Painting Picking Up
    bool isPaintHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
        if (isPaintHit && Input.GetKeyTriggered(KeyCode.E))
        {
      playerMove.FreezePlayer();
      isBeingPickedUp = true;
      EToPickUpUI.SetActive(false);
      return;
    }
        EToPickUpUI.SetActive(isPaintHit);
    }
}

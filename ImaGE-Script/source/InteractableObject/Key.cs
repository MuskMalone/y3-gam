using IGE.Utils;

public class Key : Entity, IInventoryItem
{
  // Script to be placed in the Key Entity (Parent)
  public Entity _Image; // Selection UI
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public KeyDoor keyDoor;
  public string keyAnimName;  // input from inspector based on name in anim component

  private bool startedAnimation = false, isPlayingAnimation = false;

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
    keyDoor.UnlockDoor();
    Destroy(mEntityID);
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    if (keyDoor.doorInteraction)
    {
      bool isKeyHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetKeyTriggered(KeyCode.E) && isKeyHit)
      {
        InternalCalls.PlaySound(mEntityID, "PickupObjects");
        inventoryScript.Additem(this);
      }
      EToPickUpUI.SetActive(isKeyHit);
      return;
    }

    // one-time flag to start animation
    if (!startedAnimation)
    {
      keyDoor.unlockDoorUI.SetActive(false);  // hide the Unlock UI during the animation
      TriggerAnimation();
      startedAnimation = true;
    }

    if (isPlayingAnimation)
    {
      // update state of animation every loop
      isPlayingAnimation = InternalCalls.IsPlayingAnimation(mEntityID);
      return;
    }

    // animation completed, remove item and trigger door unlock
    inventoryScript.RemoveItem(this);
  }

  void TriggerAnimation()
  {
    SetActive(true);
    InternalCalls.PlayAnimation(mEntityID, keyAnimName);
    isPlayingAnimation = true;
  }
}

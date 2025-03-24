using IGE.Utils;
using System.Numerics;

public class Key : Entity, IInventoryItem
{
  // Script to be placed in the Key Entity (Parent)
  public Entity _Image; // Selection UI
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public KeyDoor keyDoor;
  public BlackBorder blackBorder;
  public Entity playerCamera;
  public Entity keyCamera;

  public string keyAnimName;  // input from inspector based on name in anim component

  private bool startedAnimation = false, isPlayingAnimation = false;
  private Vector3 startPos;
  private Vector3 zoomInPos;
  float elapsedTime = 0.0f;
  float zoomInDuration = 2.0f;
  bool isZoomingIn = true;

  private bool isBeingPickedUp = false;

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
    Destroy();
  }

  void Start()
  {
    if (keyCamera == null)
    {
      Debug.LogError("[Key.cs] Key Camera not found");
      return;
    }

    startPos = InternalCalls.GetPosition(keyCamera.mEntityID);
    /*
    targetPos = startPos - InternalCalls.GetMainCameraDirection(keyCamera.mEntityID) * 5.0f;
    */

    zoomInPos = startPos + new Vector3(2, 0, 0);
    
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
      }
      return;
    }

    if (keyDoor.doorInteraction)
    {
      bool isKeyHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetKeyTriggered(KeyCode.E) && isKeyHit)
      {
        isBeingPickedUp = true;
      }
      EToPickUpUI.SetActive(isKeyHit);
      return;
    }

    // one-time flag to start animation
    if (!startedAnimation)
    {
      if (inventoryScript.isVisible)
      {
        inventoryScript.ToggleInventoryVisibility();
      }
      blackBorder.DisplayBlackBorders();
      keyDoor.unlockDoorUI.SetActive(false);  // hide the Unlock UI during the animation
      TriggerAnimation();
      startedAnimation = true;
      InternalCalls.PlaySound(mEntityID, "KeyUnlock");
    }

    if (isPlayingAnimation)
    {
      // update state of animation every loop
      isPlayingAnimation = InternalCalls.IsPlayingAnimation(mEntityID);

      if (isZoomingIn)
      {
        elapsedTime += Time.deltaTime;
        float t = elapsedTime / zoomInDuration;
        t = t * t * (3 - 2 * t); // SmoothStep easing
        Vector3 newPos = Vector3.Lerp(startPos, zoomInPos, t);
        InternalCalls.SetPosition(keyCamera.mEntityID, ref newPos);

        if (elapsedTime >= zoomInDuration)
        {
          elapsedTime = 0.0f;
          isZoomingIn = false;
        }
      }

      return;
    }

    // animation completed, remove item and trigger door unlock
    InternalCalls.StopSound(mEntityID, "KeyUnlock");
    inventoryScript.RemoveItem(this);
  }

  void TriggerAnimation()
  {
    SetActive(true);
    InternalCalls.PlayAnimation(mEntityID, keyAnimName);
    isPlayingAnimation = true;
    SetKeyCameraAsMain();
  }

  private void SetKeyCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "PlayerCamera");
    InternalCalls.SetTag(keyCamera.mEntityID, "MainCamera");
  }
}

using IGE.Utils;
using System;
using static Dialogue;

public class HammerLevel3 : Entity, IInventoryItem
{
  // Script to be placed in the Hammer Entity (Parent)
  public Entity _Image; // Selection UI
  public Level3Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public BlackBorder blackBorder;
  public Entity hammerCamera;
  public Entity playerCamera;

  //public Dialogue dialogueSystem;
  //public string[] hammerDialogue;

  public Entity Nail1, Nail2;
  public Entity Nail3;
  public Entity Nail4, Nail5;
  public String[] animations; // 1 animation for each plank

  private Entity[] nails; // nails in sets of 2
  private int currIndex = -1;

  private int nailCount = 0;
  private int delayCount = 0;
  private float[] sfxDelays = new float[] { 0f, 1.5f, 1.3f, .8f, 1.2f};
  private float sfxTimeElapsed = 0f;
  private bool startSFX = false;

  private bool isBeingPickedUp = false;
  public float finalDistanceAwayFromCamAfterPickup = 2f;
  public PlayerMove playerMove;

  public enum HammerState
  {
    IDLE,
    PICKED_UP,
    USING,
    COMPLETE
  }
  private HammerState currState = HammerState.IDLE;

  public string Name
  {
    get
    {
      return "Hammer";
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
    currState = HammerState.PICKED_UP;
    SetActive(false);
  }

  public void OnUsed()
  {
    SetActive(true);
    blackBorder.DisplayBlackBorders();
    SetHammerCameraAsMain();
    currState = HammerState.USING;
    if (inventoryScript.isVisible)
    {
      inventoryScript.ToggleInventoryVisibility();
    }
  }

  public void PlayNailSound()
  {
    nailCount %= 4;
    InternalCalls.SetSoundVolume(mEntityID, $"..\\Assets\\Audio\\NailPull{nailCount+1}_SFX.wav", 0.2f);
    InternalCalls.PlaySound(mEntityID, $"..\\Assets\\Audio\\NailPull{nailCount+1}_SFX.wav");
    nailCount++;
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);

    nails = new Entity[] {
      Nail1, Nail2,  // plank 1
      Nail3, null,    // plank 2 only has 1 nail
      Nail4, Nail5    // plank 3
    };
  }

  void Update()
  {
    Console.WriteLine($"{sfxTimeElapsed} |||| {delayCount} ||||| {startSFX}");
    if (startSFX && delayCount < 5)
    {
      if (sfxTimeElapsed < sfxDelays[delayCount])
      {
        sfxTimeElapsed += InternalCalls.GetDeltaTime();
      }
      else
      {
        PlayNailSound();
        sfxTimeElapsed = 0f;
        delayCount++;
      }
    }

    switch (currState)
    {
      case HammerState.IDLE:
        {
          if (isBeingPickedUp)
          {
            if (Pickup.MoveAndShrink(this, playerInteraction.mEntityID, playerCamera.mEntityID, finalDistanceAwayFromCamAfterPickup))
            {
              InternalCalls.PlaySound(mEntityID, "PickupObjects");
              isBeingPickedUp = false;
              playerMove.UnfreezePlayer();
              inventoryScript.Additem(this);
            }
            return;
          }

          bool isHammerHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
          if (Input.GetKeyTriggered(KeyCode.E) && isHammerHit)
          {
            EToPickUpUI.SetActive(false);
            isBeingPickedUp = true;
            playerMove.FreezePlayer();
            return;
          }

          EToPickUpUI.SetActive(isHammerHit);

          break;
        }

      case HammerState.PICKED_UP:

        break;

      case HammerState.USING:
        {
          startSFX = true;
          
          if (InternalCalls.IsPlayingAnimation(mEntityID)) { return; }

          // if no more animations, go back to inactive
          if (++currIndex >= animations.Length)
          {
            SetActive(false);
            blackBorder.HideBlackBorders();
            SetPlayerCameraAsMain();
            currState = HammerState.COMPLETE;
            return;
          }

          // trigger hammer anim
          InternalCalls.PlayAnimation(mEntityID, animations[currIndex]);

          // also trigger the anim for both nails
          int offset = currIndex * 2;

          //PlayNailSound();
          nails[offset]?.FindScript<Nail>().TriggerAnim();
          if (offset == 0)
          {
            //PlayNailSound();
            nails[offset + 1]?.FindScript<TwoPlankNail>().TriggerAnim();
          }
          else
          {
            //PlayNailSound();
            nails[offset + 1]?.FindScript<Nail>().TriggerAnim();
          }

          break;
        }

      default:
        return;
    } // end switch (currState)

  }

  public HammerState GetState() { return currState; }

  private void SetHammerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "PlayerCamera");
    InternalCalls.SetTag(hammerCamera.mEntityID, "MainCamera");
  }

  private void SetPlayerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(hammerCamera.mEntityID, "HammerDoorCamera");
  }
}

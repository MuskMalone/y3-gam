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
  //public Dialogue dialogueSystem;
  //public string[] hammerDialogue;

  public Entity Nail1, Nail2;
  public Entity Nail3;
  public Entity Nail4, Nail5;
  public String[] animations; // 1 animation for each plank

  private Entity[] nails; // nails in sets of 2
  private int currIndex = -1;

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
    currState = HammerState.USING;
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
    switch (currState)
    {
      case HammerState.IDLE:
        {
          bool isHammerHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
          if (Input.GetKeyTriggered(KeyCode.E) && isHammerHit)
          {
            InternalCalls.PlaySound(mEntityID, "PickupObjects");
            inventoryScript.Additem(this);
            EToPickUpUI.SetActive(false);
            return;
          }

          EToPickUpUI.SetActive(isHammerHit);

          break;
        }

      case HammerState.PICKED_UP:

        break;

      case HammerState.USING:
        {
          if (InternalCalls.IsPlayingAnimation(mEntityID)) { return; }

          // if no more animations, go back to inactive
          if (++currIndex >= animations.Length)
          {
            SetActive(false);
            currState = HammerState.COMPLETE;
            return;
          }


          // trigger hammer anim
          InternalCalls.PlayAnimation(mEntityID, animations[currIndex]);

          // also trigger the anim for both nails
          int offset = currIndex * 2;
          nails[offset]?.FindScript<Nail>().TriggerAnim();
          if (offset == 0)
          {
            nails[offset + 1]?.FindScript<TwoPlankNail>().TriggerAnim();
          }
          else
          {
            nails[offset + 1]?.FindScript<Nail>().TriggerAnim();
          }

          break;
        }

      default:
        return;
    } // end switch (currState)
  }

  public HammerState GetState() { return currState; }
}

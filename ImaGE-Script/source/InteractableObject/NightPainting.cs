using IGE.Utils;
using static Dialogue;

public class NightPainting : Entity, IInventoryItem
{
  public Entity _Image;
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Dialogue dialogueSystem;
  public string[] pitPaintingDialogue;

  private bool isBeingPickedUp = false;
  public Entity playerCamera;
  public PlayerMove playerMove;

  private bool isNightPaintingDialogueActive = false;

    private bool hasPlayedLine0Sound = false;
    private bool hasPlayedLine1Sound = false;

    public string Name
  {
    get
    {
      return "NightPainting";
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
      if (Pickup.MoveAndShrink(this, playerInteraction.mEntityID, playerCamera.mEntityID))
      {
        InternalCalls.PlaySound(mEntityID, "PickupPainting");
        isBeingPickedUp = false;
        playerMove.UnfreezePlayer();
        inventoryScript.Additem(this);
        dialogueSystem.SetDialogue(pitPaintingDialogue, new Dialogue.Emotion[] { Emotion.Surprised, Emotion.Shocked });
        isNightPaintingDialogueActive = true;
      }
      return;
    }

    // For Painting Picking Up
    bool isPaintHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (isPaintHit && Input.GetKeyTriggered(KeyCode.E))
    {
      isBeingPickedUp = true;
      playerMove.FreezePlayer();
      EToPickUpUI.SetActive(false);
      return;
    }
    EToPickUpUI.SetActive(isPaintHit);

        //if (isNightPaintingDialogueActive && dialogueSystem.CurrentLineIndex == 0)
        //{
        //    InternalCalls.PlaySound(mEntityID, "L1_4");
        //    isNightPaintingDialogueActive = false;
        //}
        //if (isNightPaintingDialogueActive && dialogueSystem.CurrentLineIndex == 1)
        //{
        //    InternalCalls.PlaySound(mEntityID, "L1_5");
        //    isNightPaintingDialogueActive = false;
        //}

        if (isNightPaintingDialogueActive)
        {
            if (dialogueSystem.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L1_4");
                hasPlayedLine0Sound = true;
            }
            else if (dialogueSystem.CurrentLineIndex == 1 && !hasPlayedLine1Sound)
            {
                InternalCalls.StopSound(mEntityID, "L1_4");
                InternalCalls.PlaySound(mEntityID, "L1_5");
                hasPlayedLine1Sound = true;
                // Optionally disable the dialogue active flag if there are no more sounds
                isNightPaintingDialogueActive = false;
            }
        }
    }
}

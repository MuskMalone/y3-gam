using IGE.Utils;

public class TutorialPainting : Entity, IInventoryItem
{
    // Script to be placed in the Tutorial Painting Entity (Parent)
    public Entity _Image;
    public TutorialLevelInventory inventoryScript;
    public PlayerInteraction playerInteraction;
    public Entity EToPickUpUI;

    private TutorialDialogue tutorialDialogue;
    public string[] dialogueWhenPaintingPickup;

    private bool isBeingPickedUp = false;
    public Entity playerCamera;
    public float finalDistanceAwayFromCamWhenPickedUp = 1.2f;
    public PlayerMove playerMove;

  public string Name
    {
        get
        {
            return "TutorialPainting";
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
        tutorialDialogue = FindObjectOfType<TutorialDialogue>();
        _Image?.SetActive(false);
        EToPickUpUI?.SetActive(false);
    }

    void Update()
    {
    if (isBeingPickedUp)
    {
      if (Pickup.MoveAndShrink(this, playerInteraction.mEntityID, playerCamera.mEntityID, finalDistanceAwayFromCamWhenPickedUp))
      {
        tutorialDialogue.SetDialogue(dialogueWhenPaintingPickup, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Thinking, TutorialDialogue.Emotion.Neutral });
        InternalCalls.PlaySound(mEntityID, "PickupObjects");
        inventoryScript.Additem(this);
        isBeingPickedUp = false;
        playerMove.UnfreezePlayer();
      }
      return;
    }
    bool isTutorialPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);

        if (Input.GetKeyTriggered(KeyCode.E) && isTutorialPaintingHit)
        {
          isBeingPickedUp = true;
          playerMove.FreezePlayer();
        }
        EToPickUpUI.SetActive(isTutorialPaintingHit);
    }
}

using IGE.Utils;

public class TutorialPainting : Entity, IInventoryItem
{
    // Script to be placed in the Tutorial Painting Entity (Parent)
    public Entity _Image;
    public TutorialLevelInventory inventoryScript;
    public PlayerInteraction playerInteraction;
    public Entity EToPickUpUI;

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
        Destroy(mEntityID);
    }

    void Start()
    {
        _Image?.SetActive(false);
        EToPickUpUI?.SetActive(false);
    }

    void Update()
    {
        bool isTutorialPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);

        if (Input.GetKeyTriggered(KeyCode.E) && isTutorialPaintingHit)
        {
            InternalCalls.PlaySound(mEntityID, "PickupObjects");
            inventoryScript.Additem(this);
        }
        EToPickUpUI.SetActive(isTutorialPaintingHit);
    }
}

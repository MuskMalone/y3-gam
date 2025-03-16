using System;
using IGE.Utils;
using static Dialogue;

public class PoseidonPainting : Entity, IInventoryItem
{
    public Entity _Image;
    public Level2Inventory level2inventoryScript;
    public PlayerInteraction playerInteraction;
    public Entity EToPickUpUI;

    public string Name
    {
        get
        {
            return "PoseidonPainting";
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
        // For Painting Picking Up
        bool isPaintHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
        if (isPaintHit && Input.GetKeyTriggered(KeyCode.E))
        {
            InternalCalls.PlaySound(mEntityID, "PickupPainting");
            level2inventoryScript.Additem(this);
        }
        EToPickUpUI.SetActive(isPaintHit);
    }
}

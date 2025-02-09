using IGE.Utils;
using System.Numerics;

public class Level2HUD : Entity
{
    public Level2Inventory inventory;

    void Start()
    {
        inventory.ItemAdded += InventoryScript_ItemAdded;
        inventory.ItemRemoved += InventoryScript_ItemRemoved;
    }

    private void InventoryScript_ItemAdded(object sender, InventoryEventArgs e)
    {
        //InternalCalls.PlaySound(mEntityID, "ItemPickup"); // Unfortunately 3D sound so HUD distance from player matters
        // Have to put in individual object scripts
        Transform inventoryPanel = GetComponent<Transform>().Find("Inventory");
        Vector3 vec3 = new Vector3(e.InventoryPosition.X, e.InventoryPosition.Y, 1f);
        InternalCalls.SetPosition(e.Item.Image.mEntityID, ref vec3);

        // If the inventory is already open, set to visible
        if (inventory.isVisible)
        {
            e.Item.Image.SetActive(true);
        }
    }

    private void InventoryScript_ItemRemoved(object sender, InventoryEventArgs e)
    {
        e.Item.Image.SetActive(false);
        e.Item = null;
    }

    void Update()
    {

    }
}

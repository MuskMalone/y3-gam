using IGE.Utils;
using System;
using System.Numerics;

public class Level4HUD : Entity
{
  public Level4Inventory inventory;

  void Start()
  {
    if (inventory != null)
    {
      inventory.ItemAdded += InventoryScript_ItemAdded;
      inventory.ItemRemoved += InventoryScript_ItemRemoved;
    }
    else
    {
      Debug.LogError("Inventory is null in Level4HUD.");
    }
  }

  private void InventoryScript_ItemAdded(object sender, InventoryEventArgs e)
  {
  //Debug.Log("InventoryScript_ItemAdded was called");
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

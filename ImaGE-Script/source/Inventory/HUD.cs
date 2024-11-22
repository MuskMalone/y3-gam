using IGE.Utils;
using System.Numerics;

public class HUD : Entity
{
  public Inventory inventory;

  // Workaround for Start() not working
  bool firstRun = true;

  void Start()
  {
    inventory.ItemAdded += InventoryScript_ItemAdded;
    inventory.ItemRemoved += InventoryScript_ItemRemoved;
  }

  private void InventoryScript_ItemAdded(object sender, InventoryEventArgs e)
  {
    Transform inventoryPanel = GetComponent<Transform>().Find("Inventory");
    Vector3 vec3 = new Vector3(e.InventoryPosition.X, e.InventoryPosition.Y, 1f);
    InternalCalls.SetPosition(e.Item.Image.mEntityID, ref vec3);
  }

  private void InventoryScript_ItemRemoved(object sender, InventoryEventArgs e)
  {
    e.Item.Image.SetActive(false);
    e.Item = null;
  }

  void Update()
  {
    // Workaround for Start() not working
    if (firstRun)
    {
      inventory.ItemAdded += InventoryScript_ItemAdded;
      inventory.ItemRemoved += InventoryScript_ItemRemoved;
      firstRun = false;
    }
  }
}

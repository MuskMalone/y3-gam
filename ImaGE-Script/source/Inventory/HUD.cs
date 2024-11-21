using IGE.Utils;
using System.Numerics;

public class HUD : Entity
{
  public Inventory inventory;

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

    //foreach (Transform slot in inventoryPanel.children)   
    //{
    //  Image image = slot.GetChild(0).GetChild(0).entity.GetComponent<Image>();            // NEED TO DO

    //  if (!image.enabled)
    //  {
    //    image.enabled = true;
    //    image.sprite = e.Item.Image;
    //    break;
    //  }
    //}

  // Existing method to handle item removal in the UI
  private void InventoryScript_ItemRemoved(object sender, InventoryEventArgs e)
  {
    Transform inventoryPanel = GetComponent<Transform>().Find("Inventory");

    //foreach (Transform slot in inventoryPanel.children)
    //{
    //  Image image = slot.GetChild(0).GetChild(0).GetComponent<Image>();

    //  // We compare the sprite or any unique identifier of the item to find it            // NEED TO DO
    //  if (image.enabled && image.sprite == e.Item.Image)
    //  {
    //    image.enabled = false;
    //    image.sprite = null;
    //    break;
    //  }
    //}
  }

  // Update is called once per frame
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

using System.Drawing.Imaging;
public class Inventory : Entity
{
  private const int SLOTS = 6;

  private List<IInventoryItem> mItems = new List<IInventoryItem>(new IInventoryItem[SLOTS]); // List with fixed size SLOTS

  public event EventHandler<InventoryEventArgs> ItemAdded;
  public event EventHandler<InventoryEventArgs> ItemRemoved;


  public Image item1;
  public Image item2;     //NEED TO DO
  public Image item3;
  public Image item4;
  public Image item5;
  public Image item6;

  public Color normalColor = Color.White;
  public Color highlightColor = Color.Yellow;

  private Image currentItem;
  private bool highlighted = false;

  public Entity pitPaintingUI;
  public Entity seedUI;
  public Entity nightPaintingUI;
  public Entity toolsPaintingUI;
  public Entity hammerUI;
  public Entity crowbarUI;
  public Entity clothUI;
  public Entity wetClothUI;

  //flags
  public bool isClothActive;
  private bool hasClothChangedToWet;

  // Start is called before the first frame update
  void Start()
  {
    currentItem = null;

    pitPaintingUI?.SetActive(false);
    seedUI?.SetActive(false);
    nightPaintingUI?.SetActive(false);
    toolsPaintingUI?.SetActive(false);
    hammerUI?.SetActive(false);
    crowbarUI?.SetActive(false);
    clothUI?.SetActive(false);
    wetClothUI?.SetActive(false);

    isClothActive = false;
    hasClothChangedToWet = false;
  }

  public void Additem(IInventoryItem item)
  {
    for (int i = 0; i < SLOTS; i++)
    {
      if (mItems[i] == null) // Check for the first available slot
      {
        mItems[i] = item;
        item.OnPickup();

        if (ItemAdded != null)
        {
          ItemAdded(this, new InventoryEventArgs(item));
        }
        return;
      }
    }

    Debug.Log("Inventory is full!");
  }

  public void RemoveItem(IInventoryItem item)
  {
    int index = mItems.IndexOf(item);
    if (index >= 0)
    {
      mItems[index] = null; // Set the slot to null instead of shifting

      item.OnUsed();

      if (ItemRemoved != null)
      {
        ItemRemoved(this, new InventoryEventArgs(item));
      }
    }
  }

  public IInventoryItem GetItemByName(string itemName)
  {
    return mItems.Find(item => item != null && item.Name == itemName);
  }

  void Update()
  {

    if (Input.anyKeyDown)
    {
      switch (Input.inputString)
      {
        case "1":
          HandleSlotInteraction(0, item1);
          break;
        case "2":
          HandleSlotInteraction(1, item2);
          break;
        case "3":
          HandleSlotInteraction(2, item3);
          break;
        case "4":
          HandleSlotInteraction(3, item4);
          break;
        case "5":
          HandleSlotInteraction(4, item5);
          break;
        case "6":
          HandleSlotInteraction(5, item6);
          break;
      }
    }

  }

  void HandleSlotInteraction(int index, Image slotImage)
  {
    if (mItems[index] != null && slotImage != null)
    {
      ToggleHighlight(slotImage);

      if (highlighted)
      {
        string itemName = mItems[index].Name;
        Debug.Log($"Item in slot {index + 1}: {itemName}");
        ShowUIForItem(itemName);
      }
      else
      {
        DisableAllUI();
      }
    }
    else
    {
      DisableAllUI();  // No item in this slot or image is null, so disable UI
    }
  }

  void ToggleHighlight(Image selectedItem)
  {
    if (currentItem != null && currentItem != selectedItem)
    {
      currentItem.color = normalColor;
      highlighted = false;
    }

    if (!highlighted || currentItem != selectedItem)
    {
      selectedItem.color = highlightColor;
      currentItem = selectedItem;
      highlighted = true;
    }
    else
    {
      selectedItem.color = normalColor;
      currentItem = null;
      highlighted = false;
    }
  }

  void ShowUIForItem(string itemName)
  {
    DisableAllUI();

    switch (itemName)
    {
      case "Pit Painting":
        pitPaintingUI?.SetActive(true);
        break;
      case "Seed":
        seedUI?.SetActive(true);
        break;
      case "NightPainting":
        nightPaintingUI?.SetActive(true);
        break;
      case "Tools Painting":
        toolsPaintingUI?.SetActive(true);
        break;
      case "Hammer":
        hammerUI?.SetActive(true);
        break;
      case "Crowbar":
        crowbarUI?.SetActive(true);
        break;
    }
  }

  void DisableAllUI()
  {
    // Ensure that we're not trying to access destroyed objects
    if (pitPaintingUI != null) pitPaintingUI.SetActive(false);
    if (seedUI != null) seedUI.SetActive(false);
    if (nightPaintingUI != null) nightPaintingUI.SetActive(false);
    if (toolsPaintingUI != null) toolsPaintingUI.SetActive(false);
    if (hammerUI != null) hammerUI.SetActive(false);
    if (crowbarUI != null) crowbarUI.SetActive(false);
    if (clothUI != null) clothUI.SetActive(false);
    if (wetClothUI != null) wetClothUI.SetActive(false);

    isClothActive = false;
  }


}

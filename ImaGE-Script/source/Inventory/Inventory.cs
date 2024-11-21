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
  private const int SLOTS = 7;
  private List<IInventoryItem> mItems = new List<IInventoryItem>(new IInventoryItem[SLOTS]);
  
  public event EventHandler<InventoryEventArgs> ItemAdded;
  public event EventHandler<InventoryEventArgs> ItemRemoved;

  private Color normalColor = Color.White;
  private Color highlightColor = Color.Yellow;

  private Image currentItem;
  private bool highlighted = false;

  public Entity pitPaintingUI;
  public Entity seedUI;
  public Entity nightPaintingUI;
  public Entity toolsPaintingUI;
  public Entity hammerUI;
  public Entity crowbarUI;
  public Entity transitionPaintingUI;
  public Entity keyUI;

  public Entity pitPaintingSelection;
  public Entity seedSelection;
  public Entity nightPaintingSelection;
  public Entity toolsPaintingSelection;
  public Entity hammerSelection;
  public Entity crowbarSelection;
  public Entity transitionPaintingSelection;
  public Entity keySelection;

  public Entity inventorySelectSquare;
  public Entity selectionHand;
  public Entity inventoryImage;

  //public Vec3<float>[] SlotPositionList;

  public bool keyEquipped;
  public bool crowbarEquipped;
  public bool hammerEquipped;
  public bool seedEquipped;

  public float startScreenPosX = -20f;
  public float endScreenPosX = -15.8f;
  private float currentPositionX;
  private float currentSlideTime = 0;
  private bool isSliding = false;
  private bool initialization = true;
  public float slideDuration = 0.5f;

  void Start()
  {
    keyEquipped = false;
    crowbarEquipped = false;
    hammerEquipped = false;
    seedEquipped = false;

    // Start with the inventory off-screen
    Vector3 originalPosition = InternalCalls.GetPosition(inventoryImage.mEntityID);
    Vector3 pos = new Vector3(startScreenPosX, originalPosition.Y, originalPosition.Z);
    InternalCalls.SetPosition(inventoryImage.mEntityID, ref pos);
    currentPositionX = startScreenPosX;

    currentItem = null;

    inventoryImage?.SetActive(false);
    pitPaintingUI?.SetActive(false);
    seedUI?.SetActive(false);
    nightPaintingUI?.SetActive(false);
    toolsPaintingUI?.SetActive(false);
    hammerUI?.SetActive(false);
    crowbarUI?.SetActive(false);
    transitionPaintingUI?.SetActive(false);
    keyUI?.SetActive(false);
    selectionHand.SetActive(false);
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

  private void ToggleInventoryVisibility()
  {
    if (isSliding) return;

    isSliding = true;
    currentSlideTime = 0;
    inventoryImage.SetActive(true);

    if (!initialization)
    {
      // Swap the Start and End Positions (Cool Syntax)
      (startScreenPosX, endScreenPosX) = (endScreenPosX, startScreenPosX);
    }
    else
      initialization = false;
  }

  void Update()
  {
    if (Input.GetKeyTriggered(KeyCode.I))
    {
      ToggleInventoryVisibility();
    }

    if (isSliding)
    {
      currentSlideTime += Time.deltaTime;

      float progress = Math.Min(currentSlideTime / slideDuration, 1.0f);
      currentPositionX = Easing.Linear(startScreenPosX, endScreenPosX, progress);

      Vector3 originalPosition = InternalCalls.GetPosition(inventoryImage.mEntityID);
      Vector3 pos = new Vector3(currentPositionX, originalPosition.Y, originalPosition.Z);
      InternalCalls.SetPosition(inventoryImage.mEntityID, ref pos);

      if (progress >= 1.0f)
      {
        isSliding = false;
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
        seedEquipped = true;
        break;
      case "NightPainting":
        nightPaintingUI?.SetActive(true);
        break;
      case "Tools Painting":
        toolsPaintingUI?.SetActive(true);
        break;
      case "Hammer":
        hammerUI?.SetActive(true);
        hammerEquipped = true;
        break;
      case "Crowbar":
        crowbarUI?.SetActive(true);
        crowbarEquipped = true;
        break;
      case "Transition Painting":
        transitionPaintingUI?.SetActive(true);
        break;
      case "Key":
        keyUI?.SetActive(true);
        keyEquipped = true;
        break;

    }
  }

  void DisableAllUI()
  {
    pitPaintingUI?.SetActive(false);
    seedUI?.SetActive(false);
    seedEquipped = false;
    nightPaintingUI?.SetActive(false);
    toolsPaintingUI?.SetActive(false);
    hammerUI?.SetActive(false);
    hammerEquipped = false;
    crowbarUI?.SetActive(false);
    crowbarEquipped = false;
    transitionPaintingUI?.SetActive(false);
    keyUI?.SetActive(false);
    keyEquipped = false;
  }
}

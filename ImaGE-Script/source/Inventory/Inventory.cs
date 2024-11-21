using System;
using System.Collections.Generic;
using System.Numerics;
using IGE.Utils;

public class Inventory : Entity
{
  private const int SLOTS = 7;
  private List<IInventoryItem> mItems = new List<IInventoryItem>(new IInventoryItem[SLOTS]);
  
  public event EventHandler<InventoryEventArgs> ItemAdded;
  public event EventHandler<InventoryEventArgs> ItemRemoved;

  private Color normalColor = Color.White;
  private Color highlightColor = Color.Yellow;

  private Entity currentItem;
  private bool highlighted = false;

  // Inventory Item UI (Image at the bottom right)
  public Entity pitPaintingUI;
  public Entity seedUI;
  public Entity nightPaintingUI;
  public Entity toolsPaintingUI;
  public Entity hammerUI;
  public Entity crowbarUI;
  public Entity transitionPaintingUI;
  public Entity keyUI;

  // Inventory Item Selection (Image in the inventory bar)
  public Entity pitPaintingSelection;
  public Entity seedSelection;
  public Entity nightPaintingSelection;
  public Entity toolsPaintingSelection;
  public Entity hammerSelection;
  public Entity crowbarSelection;
  public Entity transitionPaintingSelection;
  public Entity keySelection;
  
  // Inventory Tools
  public Entity inventorySelectSquare;
  public Entity selectionHand;
  public Entity inventoryImage;

  //public Vec3<float>[] SlotPositionList;
  // Workaround for lack of Vec3<float>[]
  private List<Vec2<float>> iconPosition = new List<Vec2<float>>
  {
      new Vec2<float>(-15.9f, 7.2f),
      new Vec2<float>(-15.9f, 4.7f),
      new Vec2<float>(-15.9f, 2.4f),
      new Vec2<float>(-15.9f, 0.1f),
      new Vec2<float>(-15.9f, -2.3f),
      new Vec2<float>(-15.9f, -4.6f),
      new Vec2<float>(-15.9f, -7.0f)
  };
  private float selectionHandXOffset = 5f;

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
  private bool isVisible = false;

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
          Debug.Log("Item has been added to inventory");
          ItemAdded(this, new InventoryEventArgs(item, iconPosition[i]));
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
        ItemRemoved(this, new InventoryEventArgs(item, iconPosition[index]));
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
    isVisible = !isVisible;
    isSliding = true;
    currentSlideTime = 0;
    inventoryImage.SetActive(true);

    if (!isVisible)
    {
      // Set Inventory Icons Inactive
      for (int i = 0; i < SLOTS; i++)
      {
        if (mItems[i] != null)
        {
          mItems[i].Image.SetActive(false);
        }
      }
    }

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

        // Set Inventory Icons Active
        if (isVisible)
        {
          for (int i = 0; i < SLOTS; i++)
          {
            if (mItems[i] != null)
            {
              mItems[i].Image.SetActive(true);
            }
          }
        }
      }
    }

    if (Input.anyKeyDown)
    {
      switch (Input.inputString)
      {
        case "1":
          HandleSlotInteraction(0, iconPosition[0], mItems[0]);
          break;
        case "2":
          HandleSlotInteraction(1, iconPosition[1], mItems[1]);
          break;
        case "3":
          HandleSlotInteraction(2, iconPosition[2], mItems[2]);
          break;
        case "4":
          HandleSlotInteraction(3, iconPosition[3], mItems[3]);
          break;
        case "5":
          HandleSlotInteraction(4, iconPosition[4], mItems[4]);
          break;
        case "6":
          HandleSlotInteraction(5, iconPosition[5], mItems[5]);
          break;
        case "7":
          HandleSlotInteraction(6, iconPosition[6], mItems[6]);
          break;
      }
    }
  }

  private void HandleSlotInteraction(int index, Vec2<float> iconPosition, IInventoryItem selectedItem)
  {
    //audioManager.PlaySFX(audioManager.moveSlotsInventory, audioManager.MoveSlotsInventoryVolume);
    ToggleHighlight(index, iconPosition, selectedItem);
    if (mItems[index] != null)
    {
      if (highlighted) // i.e. Item is selected
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
      DisableAllUI();
    }
  }

  private void ToggleHighlight(int index, Vec2<float> iconPosition, IInventoryItem selectedItem)
  {
    /*
    if (currentItem != null && currentItem != selectedItem)
    {
      inventoryImage.sprite = defaultSprite;
      selectionHand.SetActive(false);
      highlighted = false;
    }

    if (!highlighted || currentItem != selectedItem)
    {
      inventoryImage.sprite = selectedSprite;

      if (isVisible)
      {
        selectionHand.SetActive(true);
        InternalCalls.SetPosition(selectionHand.mEntityID, 
          new Vector3())
        selectionHand.transform.position = new Vector3(paw.transform.position.x, selectedItem.transform.position.y, paw.transform.position.z);
      }



      currentItem = selectedItem;
      highlighted = true;
    }
    else
    {
      inventoryImage.sprite = defaultSprite;
      paw.SetActive(false);
      currentItem = null;
      highlighted = false;
    }
    */
  }

  private void ShowUIForItem(string itemName)
  {
    DisableAllUI();

    switch (itemName)
    {
      case "Pit Painting":
        pitPaintingUI?.SetActive(true);
        break;
      case "Seed":
        Debug.Log("Seed Equipped");
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

  private void DisableAllUI()
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

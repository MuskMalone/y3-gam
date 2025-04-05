using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using IGE.Utils;

public class Level4Inventory : Entity
{
  private const int SLOTS = 7;
  private List<IInventoryItem> mItems = new List<IInventoryItem>(new IInventoryItem[SLOTS]);

  public event EventHandler<InventoryEventArgs> ItemAdded;
  public event EventHandler<InventoryEventArgs> ItemRemoved;

  private IInventoryItem currentItem;
  public bool highlighted = false;
  public bool isVisible = false;

  // Inventory Item UI (Image at the bottom right)
  public Entity momPaintingUI;

  // Inventory Item Selection (Image in the inventory bar)
  public Entity momPaintingSelection;

  // Inventory Tools
  public Entity inventorySelectSquare;
  public Entity selectionHand;
  public Entity inventoryImage;
  private PictureAlign pictureAlignscript;

  //public Vec3<float>[] SlotPositionList;
  // Workaround for lack of Vec3<float>[]
  private readonly List<Vec2<float>> iconPosition = new List<Vec2<float>>
  {
      new Vec2<float>(-14.745f, 6.530f),
      new Vec2<float>(-15.140f, 4.259f),
      new Vec2<float>(-15.241f, 1.980f),
      new Vec2<float>(-15.290f, -0.320f),
      new Vec2<float>(-15.340f, -2.599f),
      new Vec2<float>(-15.190f, -4.890f),
      new Vec2<float>(-14.869f, -7.180f)
  };

  // Sliding Inventory
  public float startScreenPosX = -20f;
  public float endScreenPosX = -15.85f;
  private float currentPositionX;
  private float currentSlideTime = 0;
  private bool isSliding = false;
  private bool initialization = true;
  public float slideDuration = 0.3f;

  // Selection Hand (Paw)
  private float selectionHandXOffset = 1.2f;
  private float currentHandTime = 0;
  public float handExpandDuration = 0.4f;
  private Vector2 startHandScale = new Vector2(1.0f, 1.2f);
  private Vector2 endHandScale = new Vector2(1.4f, 1.8f);
  private Vector2 currentHandScale;

  //Painting Alignment UI
  private PaintingAlignUILevel4 paintingAlignUILevel4Script;

  void Start()
  {

    // Start with the inventory off-screen
    Vector3 originalPosition = InternalCalls.GetPosition(inventoryImage.mEntityID);
    Vector3 pos = new Vector3(startScreenPosX, originalPosition.Y, originalPosition.Z);
    InternalCalls.SetPosition(inventoryImage.mEntityID, ref pos);
    currentPositionX = startScreenPosX;

    currentItem = null;

    inventoryImage?.SetActive(false);
    selectionHand?.SetActive(false);

    momPaintingUI?.SetActive(false);


    Vector3 startPosition = new Vector3(12.0f, -4.0f, 0f);
    InternalCalls.SetPosition(momPaintingUI.mEntityID, ref startPosition);
    //InternalCalls.SetPosition(hammerUI.mEntityID, ref startPosition);

    pictureAlignscript = FindObjectOfType<PictureAlign>();

    paintingAlignUILevel4Script = FindObjectOfType<PaintingAlignUILevel4>();
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
    currentItem = null;
    highlighted = false;
    selectionHand.SetActive(false);
    inventorySelectSquare.SetActive(false);
    DisableAllUI();

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

  public void ToggleInventoryVisibility()
  {
    if (isSliding) return;
    isVisible = !isVisible;
    isSliding = true;
    currentSlideTime = 0;
    inventoryImage.SetActive(true);
    InternalCalls.PlaySound(mEntityID, "InventoryPopUp");

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

      selectionHand.SetActive(false);
      inventorySelectSquare.SetActive(false);
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

    // if (!pictureAlignscript.isFading && Input.anyKeyTriggered)
    if (Input.anyKeyTriggered)
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

    if (selectionHand.IsActive())
    {
      currentHandTime += Time.deltaTime;

      float progress = Math.Min(currentHandTime / handExpandDuration, 1.0f);
      currentHandScale = Easing.Linear(startHandScale, endHandScale, progress);

      Vector3 originalScale = InternalCalls.GetScale(selectionHand.mEntityID);
      Vector3 newScale = new Vector3(currentHandScale.X, currentHandScale.Y, originalScale.Z);
      InternalCalls.SetScale(selectionHand.mEntityID, ref newScale);

      if (progress >= 1.0f)
      {
        (startHandScale, endHandScale) = (endHandScale, startHandScale);
        currentHandTime = 0f;
      }
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

          if (highlighted && currentItem != null)
          {
            selectionHand.SetActive(true);
            inventorySelectSquare.SetActive(true);
          }
        }
      }
    }
  }

  private void HandleSlotInteraction(int index, Vec2<float> iconPosition, IInventoryItem selectedItem)
  {
    InternalCalls.PlaySound(mEntityID, "MoveSlots");
    ToggleHighlight(iconPosition, selectedItem);
    if (mItems[index] != null)
    {
      if (highlighted) // i.e. Item is selected
      {
        string itemName = mItems[index].Name;
        Debug.Log($"Item in slot {index + 1}: {itemName}");
        Debug.Log("Item selected");
        ShowUIForItem(itemName);
      }
      else
        DisableAllUI();
    }
    else
      DisableAllUI();
  }

  private void ToggleHighlight(Vec2<float> iconPosition, IInventoryItem selectedItem)
  {
    if (currentItem != null && currentItem != selectedItem)
    {
      inventorySelectSquare.SetActive(false);
      selectionHand.SetActive(false);
      highlighted = false;
    }

    if (!highlighted || currentItem != selectedItem)
    {
      Vector3 inventorySelectPosition = new Vector3(iconPosition.X, iconPosition.Y, 0.5f);
      InternalCalls.SetPosition(inventorySelectSquare.mEntityID, ref inventorySelectPosition);

      Vector3 handPosition = new Vector3(iconPosition.X + selectionHandXOffset, iconPosition.Y, 1f);
      InternalCalls.SetPosition(selectionHand.mEntityID, ref handPosition);

      if (isVisible)
      {
        selectionHand.SetActive(true);
        inventorySelectSquare.SetActive(true);
      }

      currentItem = selectedItem;
      highlighted = true;
    }
    else
    {
      inventorySelectSquare.SetActive(false);
      selectionHand.SetActive(false);
      highlighted = false;
      currentItem = null;
    }
  }

  private void ShowUIForItem(string itemName)
  {
    DisableAllUI();

    switch (itemName)
    {
      case "MotherBehindGatePainting":
        momPaintingUI?.SetActive(true);
        momPaintingUI?.FindScript<HoldupUI>().SetAlginUI("MotherBehindGatePainting", GetItemByName("MotherBehindGatePainting"));
        paintingAlignUILevel4Script.isPainting = true;
        break;
    }
  }

  private void DisableAllUI()
  {
    momPaintingUI?.SetActive(false);
    pictureAlignscript.ClearUI();
    paintingAlignUILevel4Script.isPainting = false;

  }

  public IInventoryItem GetCurrentItem()
  {
    return currentItem;
  }

}

using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using IGE.Utils;

public class Level2Inventory : Entity
{
    private const int SLOTS = 7;
    private List<IInventoryItem> mItems = new List<IInventoryItem>(new IInventoryItem[SLOTS]);

    public event EventHandler<InventoryEventArgs> ItemAdded;
    public event EventHandler<InventoryEventArgs> ItemRemoved;

    private IInventoryItem currentItem;
    public bool highlighted = false;
    public bool isVisible = false;

    // Inventory Item UI (Image at the bottom right)
    public Entity dionysusPaintingUI;
    public Entity artemisPaintingUI;
    public Entity zeusPaintingUI;
    public Entity poseidonPaintingUI;
    public Entity twotoonePaintingUI;
    public Entity onetofivePaintingUI;
    public Entity onetosixPaintingUI;
    public Entity threetoonePaintingUI;
    public Entity fourtoonePaintingUI;
    public Entity sixtofourPaintingUI;
    public Entity sixtosevenPaintingUI;
    public Entity seventothreePaintingUI;

    // Inventory Item Selection (Image in the inventory bar)
    public Entity dionysusPaintingSelection;
    public Entity artemisPaintingSelection;
    public Entity zeusPaintingSelection;
    public Entity poseidonPaintingSelection;
    public Entity twotoonePaintingSelection;
    public Entity onetofivePaintingSelection;
    public Entity onetosixPaintingSelection;
    public Entity threetoonePaintingSelection;
    public Entity fourtoonePaintingSelection;
    public Entity sixtofourPaintingSelection;
    public Entity seventothreePaintingSelection;

    // Inventory Tools
    public Entity inventorySelectSquare;
    public Entity selectionHand;
    public Entity inventoryImage;
    private PictureAlign pictureAlignscript;

    private PaintingAlignUILevel2 paintingAlignUILevel2Script;

    //public Vec3<float>[] SlotPositionList;
    // Workaround for lack of Vec3<float>[]
    private List<Vec2<float>> iconPosition = new List<Vec2<float>>
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

        dionysusPaintingUI?.SetActive(false);


        Vector3 startPosition = new Vector3(12.0f, -4.0f, 0f);
        InternalCalls.SetPosition(dionysusPaintingUI.mEntityID, ref startPosition);

        pictureAlignscript = FindObjectOfType<PictureAlign>();
        paintingAlignUILevel2Script = FindObjectOfType<PaintingAlignUILevel2>();
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

    // i'm accounting for the fact that you can solve the key code without collecting
    // all paintings and by guessing the last number, meaning it is possible for someone
    // to walk back out of the hex room to collect other paintings after the door opens.
    // We will only clear non-HexPaintings from inventory via a trigger
    public void ClearInventoryForHexRoom()
    {
      // Create a copy of the list to iterate over
      var mItemsCopy = new List<IInventoryItem>(mItems);

      foreach (IInventoryItem item in mItemsCopy)
      {
        if (item != null && !item.Name.StartsWith("HexPainting"))
        {
          Debug.Log("Removing Item: " + item.Name);
          RemoveItem(item);
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

        if (!pictureAlignscript.isFading && Input.anyKeyTriggered)
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
            case "AthenaPainting":
                dionysusPaintingUI?.SetActive(true);
                dionysusPaintingUI?.FindScript<HoldupUI>().SetAlginUI("AthenaPainting", GetItemByName("AthenaPainting"));
                paintingAlignUILevel2Script.isPainting = true;
                break;

            case "HermesPainting":
                artemisPaintingUI?.SetActive(true);
                artemisPaintingUI?.FindScript<HoldupUI>().SetAlginUI("HermesPainting", GetItemByName("HermesPainting"));
                paintingAlignUILevel2Script.isPainting = true;
                break;

            case "ZeusPainting":
                zeusPaintingUI?.SetActive(true);
                zeusPaintingUI?.FindScript<HoldupUI>().SetAlginUI("ZeusPainting", GetItemByName("ZeusPainting"));
                paintingAlignUILevel2Script.isPainting = true;
                break;

            case "PoseidonPainting":
                poseidonPaintingUI?.SetActive(true);
                poseidonPaintingUI?.FindScript<HoldupUI>().SetAlginUI("PoseidonPainting", GetItemByName("PoseidonPainting"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingDestructible2to1":
                twotoonePaintingUI?.SetActive(true);
                twotoonePaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingDestructible2to1", GetItemByName("HexPaintingDestructible2to1"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible1to5":
                onetofivePaintingUI?.SetActive(true);
                onetofivePaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible1to5", GetItemByName("HexPaintingIndestructible1to5"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible1to6":
                onetosixPaintingUI?.SetActive(true);
                onetosixPaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible1to6", GetItemByName("HexPaintingIndestructible1to6"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible3to1":
                threetoonePaintingUI?.SetActive(true);
                threetoonePaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible3to1", GetItemByName("HexPaintingIndestructible3to1"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible4to1":
                fourtoonePaintingUI?.SetActive(true);
                fourtoonePaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible4to1", GetItemByName("HexPaintingIndestructible4to1"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible6to4":
                sixtofourPaintingUI?.SetActive(true);
                sixtofourPaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible6to4", GetItemByName("HexPaintingIndestructible6to4"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible6to7":
                sixtosevenPaintingUI?.SetActive(true);
                sixtosevenPaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible6to7", GetItemByName("HexPaintingIndestructible6to7"));
                paintingAlignUILevel2Script.isPainting = true;
                break;
            case "HexPaintingIndestructible7to3":
                seventothreePaintingUI?.SetActive(true);
                seventothreePaintingUI?.FindScript<HoldupUI>().SetAlginUI("HexPaintingIndestructible7to3", GetItemByName("HexPaintingIndestructible7to3"));
                paintingAlignUILevel2Script.isPainting = true;
                break;

        }
    }

    private void DisableAllUI()
    {
        dionysusPaintingUI?.SetActive(false);
        artemisPaintingUI?.SetActive(false);
        zeusPaintingUI?.SetActive(false);
        poseidonPaintingUI?.SetActive(false);
        twotoonePaintingUI?.SetActive(false);
        onetofivePaintingUI?.SetActive(false);
        onetosixPaintingUI?.SetActive(false);
        threetoonePaintingUI?.SetActive(false);
        fourtoonePaintingUI?.SetActive(false);
        sixtofourPaintingUI?.SetActive(false);
        sixtosevenPaintingUI?.SetActive(false);
        seventothreePaintingUI?.SetActive(false);
        paintingAlignUILevel2Script.isPainting = false;
        pictureAlignscript.ClearUI();

    }

    public IInventoryItem GetCurrentItem()
    {
        return currentItem;
    }

    public bool HasAllPaintings()
    {
        for (int i = 0; i < 7; i++)
        {
            if (mItems[i] == null)
            {
                return false;
            }
        }
        return true;
    }

}

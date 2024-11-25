using IGE.Utils;
using System;

public interface IInventoryItem
{
  string Name { get; }
  Entity Image { get; set; } // This should be a UI (Canvas) child entity that
                             // has the UI Image component
  void OnPickup();
  void OnUsed();
}

public class InventoryEventArgs : EventArgs
{
  public InventoryEventArgs(IInventoryItem item, Vec2<float> inventoryPosition)
  {
    Item = item;
    InventoryPosition = inventoryPosition;
  }

  public IInventoryItem Item;
  public Vec2<float> InventoryPosition;
}
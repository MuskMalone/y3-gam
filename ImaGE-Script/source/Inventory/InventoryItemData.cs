using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

using System.Drawing.Imaging;


public interface IInventoryItem
{
  string Name { get; }
  Entity Image { get; } // This should be a UI (Canvas) child entity that
                        // has the UI Image component
  void OnPickup();
  void OnUsed();
}

public class InventoryEventArgs : EventArgs
{
  public InventoryEventArgs(IInventoryItem item)
  {
    Item = item;
  }

  public IInventoryItem Item;
}
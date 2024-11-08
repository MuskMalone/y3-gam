using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
using System.Drawing;      // For Bitmap (or use other graphics libraries)
using System.Drawing.Imaging;


public interface IInventoryItem
{
  string Name { get; }
  //Sprite Image { get; }
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
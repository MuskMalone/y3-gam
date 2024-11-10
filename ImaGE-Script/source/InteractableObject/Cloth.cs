using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
using System.Drawing;      // For Bitmap (or use other graphics libraries)
using System.Drawing.Imaging;

public class Cloth : Entity, IInventoryItem
{
  public string Name
  {
    get
    {
      return "Cloth";
    }
  }

  public Sprite _Image = null;

  public Sprite Image
  {
    get
    {
      return _Image;
    }
  }


  public void OnPickup()
  {
    SetActive(false);

  }

  public void OnUsed()
  {
    Destroy(mEntityID);
  }
}

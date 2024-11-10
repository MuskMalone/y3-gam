using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
using System.Drawing;      // For Bitmap (or use other graphics libraries)
using System.Drawing.Imaging;

public class NightPainting : Entity, IInventoryItem
{
  public string Name
  {
    get
    {
      return "NightPainting";
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

  //public void OnPickup()
  //{
  //    Destroy(Entity);
  //}

  public void OnPickup()
  {
    //Destroy(Entity); 
    SetActive(false);

  }

  public void OnUsed()
  {
    Destroy(mEntityID);
  }
}

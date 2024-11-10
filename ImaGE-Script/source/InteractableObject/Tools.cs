using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

using System.Drawing.Imaging;
public class Tools : Entity, IInventoryItem
{
  public string Name
  {
    get
    {
      return "Tools Painting";
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

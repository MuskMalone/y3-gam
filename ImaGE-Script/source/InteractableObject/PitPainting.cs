using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

using System.Drawing.Imaging;

public class PitPainting : Entity, IInventoryItem
{
  public string Name
  {
    get
    {
      return "Pit Painting";
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
    //Destroy(Entity); 
    SetActive(false);

  }

  public void OnUsed()
  {
    Destroy(mEntityID);
  }

}

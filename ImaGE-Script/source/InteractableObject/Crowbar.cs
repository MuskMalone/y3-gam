using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

using System.Drawing.Imaging;
public class Crowbar : Entity, IInventoryItem
{
  public string Name
  {
    get
    {
      return "Crowbar";
    }
  }

  public Entity _Image;

  public Entity Image
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

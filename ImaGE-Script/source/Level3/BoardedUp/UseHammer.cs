using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class UseHammer : Entity
{
  public Entity Nail1, Nail2;
  public Entity Nail3;
  public Entity Nail4, Nail5;
  public String[] animations; // 1 animation for each plank

  private Entity[] nails; // nails in sets of 2
  private int currIndex = -1;

  // Start is called before the first frame update
  void Start()
  {
    nails = new Entity[] {
      Nail1, Nail2,  // plank 1
      Nail3, null,    // plank 2 only has 1 nail
      Nail4, Nail5    // plank 3
    };
  }

  // Update is called once per frame
  void Update()
  {
    
  }
}



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

public class TransitionHallwayTrigger : Entity
{
  public Entity player;
  public Entity transitionHallway;  // parent of the entire vertical corridor

  // Start is called before the first frame update
  void Start()
  {
    transitionHallway.SetActive(false);
  }

  // Update is called once per frame
  void Update()
  {
    // visible in the corridor and not visible from the outside
    if (InternalCalls.OnTriggerEnter(mEntityID, player.mEntityID))
    {
      transitionHallway.SetActive(true);
    }
    else if (InternalCalls.OnTriggerExit(mEntityID, player.mEntityID))
    {
      transitionHallway.SetActive(false);
    }
  }

  // called when the doors have slammed and the player can
  // no longer come out of the corridor
  public void PlayerTrapped()
  {
    // the hallway should be permanently visible from this point on
    transitionHallway.SetActive(true);
    Destroy();
  }
}

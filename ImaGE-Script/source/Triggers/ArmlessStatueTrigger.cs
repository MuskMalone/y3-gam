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

public class ArmlessStatueTrigger : Entity
{
  public Entity Player;
  public Entity Head, Tears;
  public string tiltAnimName, tearsAnimName;

  private enum State
  {
    WAITING,
    HEAD_TILT
  }
  private State currState = State.WAITING;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      case State.WAITING:
        // if player enters box, trigger first animation
        if (InternalCalls.OnTriggerEnter(mEntityID, Player.mEntityID))
        {
          InternalCalls.PlayAnimation(Head.mEntityID, tiltAnimName);
          currState = State.HEAD_TILT;
        }
        break;

      case State.HEAD_TILT:
        if (InternalCalls.IsPlayingAnimation(Head.mEntityID)) { return; }

        // after first animation, trigger 2nd and destroy self
        InternalCalls.PlayAnimation(Tears.mEntityID, tearsAnimName);
        Destroy();

        break;
    }
  }
}

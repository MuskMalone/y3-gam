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
  public Entity ShoulderCollider1, ShoulderCollider2;
  public Entity FloorCollider;
  public string tiltAnimName, tearsAnimName;

  private enum State
  {
    WAITING,
    HEAD_TILT,
    TEARS,
    FALLING
  }
  private State currState = State.WAITING;

  // Start is called before the first frame update
  void Start()
  {
    ShoulderCollider1.SetActive(false);
    ShoulderCollider2.SetActive(false);
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
        if (InternalCalls.IsPlayingAnimation(Head.mEntityID))
        {
          InternalCalls.UpdatePhysicsToTransform(Head.mEntityID);
          return;
        }

        // after first animation, trigger tears falling
        InternalCalls.PlayAnimation(Tears.mEntityID, tearsAnimName);
        currState = State.TEARS;


        break;

      case State.TEARS:
        if (InternalCalls.IsPlayingAnimation(Tears.mEntityID)) { return; }

        // activate support colliders
        ShoulderCollider1.SetActive(true);
        ShoulderCollider2.SetActive(true);

        // now the head needs to fall. Let gravity do its work
        InternalCalls.SetGravityFactor(Head.mEntityID, 10f);
        InternalCalls.LockRigidBody(Head.mEntityID, false);
        currState = State.FALLING;

        break;

      case State.FALLING:
        // check for head's collision with ground
        if (InternalCalls.GetContactPoints(FloorCollider.mEntityID, Head.mEntityID).Length > 0)
        {
          InternalCalls.LockRigidBody(Head.mEntityID, true);  // prevent it from moving further
          CleanUp();  // we are done
        }

        break;
    }
  }

  private void CleanUp()
  {
    InternalCalls.DestroyEntity(ShoulderCollider1.mEntityID);
    InternalCalls.DestroyEntity(ShoulderCollider2.mEntityID);
    Destroy();
  }
}

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
          Head.GetComponent<Animation>().Play(tiltAnimName);
          currState = State.HEAD_TILT;
          InternalCalls.PlaySound(Head.mEntityID, "..\\Assets\\Audio\\HeadMoving_SFX.wav");
          InternalCalls.StopSound(Head.mEntityID, "..\\Assets\\Audio\\GhostlyFemaleSinging_SFX.wav");
        }
        break;

      case State.HEAD_TILT:
        if (Head.GetComponent<Animation>().IsPlaying())
        {
          InternalCalls.UpdatePhysicsToTransform(Head.mEntityID);
          return;
        }

        // after first animation, trigger tears falling
        Tears.GetComponent<Animation>().Play(tearsAnimName);
        currState = State.TEARS;
        InternalCalls.PlaySound(Head.mEntityID, "..\\Assets\\Audio\\StatueCrying_SFX.wav");

        break;

      case State.TEARS:
        if (Tears.GetComponent<Animation>().IsPlaying()) { return; }

        // activate support colliders
        ShoulderCollider1.SetActive(true);
        ShoulderCollider2.SetActive(true);

        // now the head needs to fall. Let gravity do its work
        InternalCalls.SetGravityFactor(Head.mEntityID, 10f);
        InternalCalls.LockRigidBody(Head.mEntityID, false);
        currState = State.FALLING;
        InternalCalls.StopSound(Head.mEntityID, "..\\Assets\\Audio\\StatueCrying_SFX.wav");
        InternalCalls.PlaySound(Head.mEntityID, "..\\Assets\\Audio\\HeadDropBefore_SFX.wav");
        break;

      case State.FALLING:
        // check for head's collision with ground
        if (InternalCalls.GetContactPoints(FloorCollider.mEntityID, Head.mEntityID).Length > 0)
        {
          InternalCalls.LockRigidBody(Head.mEntityID, true);  // prevent it from moving further
          CleanUp();  // we are done
          InternalCalls.PlaySound(Head.mEntityID, "..\\Assets\\Audio\\HeadImpact_SFX.wav");
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

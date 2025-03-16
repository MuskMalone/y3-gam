using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

// to be attached to every nail holding 1 board in Level3
public class Nail : Entity
{
  public int nailId;
  public Entity plankHeld;
  public float timeTillDespawn;

  private enum State
  {
    IN_PLANK,
    ANIMATION,
    FALLEN
  }
  private State currState = State.IN_PLANK;
  private string dropAnimName = "Drop";
  private float timeElapsed = 0f;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      // not yet activated
      case State.IN_PLANK:
        return;

      case State.ANIMATION:
      {
        // update collider to transform during animation
        if (InternalCalls.IsPlayingAnimation(mEntityID))
        {
          InternalCalls.UpdatePhysicsToTransform(mEntityID);
        }
        // unlock the rigidbody when complete
        else
        {
          InternalCalls.LockRigidBody(mEntityID, false);
          InternalCalls.SetGravityFactor(mEntityID, 10.0f);

          NailDropped();

          currState = State.FALLEN;
        }

        break;
      }
      case State.FALLEN:
      {
        timeElapsed += InternalCalls.GetDeltaTime();

        if (timeElapsed >= timeTillDespawn)
        {
          Despawn();
        }

        break;
      }
    } // end switch(currState)
  }

  private void NailDropped()
  {
    switch (nailId)
    {
      // cause one side of plank 1 to fall
      case 1:
        // drop 1 side of Plank1 by unlocking rotation and simulating gravity
        InternalCalls.SetAngularVelocity(plankHeld.mEntityID, new Vector3(1f / InternalCalls.GetDeltaTime(), 5f, 5f));
        InternalCalls.LockRigidBodyRotation(plankHeld.mEntityID, false, false, false);
        break;

      case 3:
        // completely drop Plank2
        InternalCalls.LockRigidBody(plankHeld.mEntityID, false);
        InternalCalls.SetGravityFactor(mEntityID, 5.0f);
        break;

      case 4:
        // drop 1 side of Plank3 by unlocking rotation and simulating gravity
        InternalCalls.SetAngularVelocity(plankHeld.mEntityID, new Vector3(1f / InternalCalls.GetDeltaTime(), 5f, 5f));
        InternalCalls.LockRigidBodyRotation(plankHeld.mEntityID, false, false, false);
        break;

      case 5:
        // completely drop Plank3
        InternalCalls.LockRigidBody(plankHeld.mEntityID, false);
        InternalCalls.SetGravityFactor(mEntityID, 5.0f);
        break;
    } // end switch(nailId)
  }

  private void Despawn()
  {
    // if 3rd or last nail, destroy the plank too
    if (nailId == 3 || nailId == 5)
    {
      InternalCalls.DestroyEntity(plankHeld.mEntityID);
    }

    Destroy(); // destroy self
  }

  public void TriggerAnim()
  {
    InternalCalls.PlayAnimation(mEntityID, dropAnimName);
    currState = State.ANIMATION;
  }
}



using IGE.Utils;
using System;
using System.Numerics;

// to be attached to the nail holding 2 boards in Level3
// i would just inherit from nail but it doesnt seem to work :(
public class TwoPlankNail : Entity
{
  public Entity Plank1, Plank2;

  private string dropAnimName = "Drop";
  public float timeTillDespawn;

  private enum State
  {
    IN_PLANK,
    ANIMATION,
    FALLEN
  }
  private State currState = State.IN_PLANK;
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

          // completely drop Plank1
          InternalCalls.LockRigidBody(Plank1.mEntityID, false);
          InternalCalls.SetGravityFactor(mEntityID, 5.0f);

          // drop 1 side of Plank2 by unlocking rotation and simulating gravity
          InternalCalls.SetAngularVelocity(Plank2.mEntityID, new Vector3(-1f / InternalCalls.GetDeltaTime(), 5f, 5f));
          InternalCalls.LockRigidBodyRotation(Plank2.mEntityID, false, false, false);

          currState = State.FALLEN;
        }

        break;
      }
      case State.FALLEN:
      {
        timeElapsed += InternalCalls.GetDeltaTime();

        // done, destroy self and 1st plank
        if (timeElapsed >= timeTillDespawn)
        {
          Destroy(Plank1.mEntityID);
          Destroy(mEntityID);
        }

        break;
      }
    } // end switch(currState)
  }
  public void TriggerAnim()
  {
    InternalCalls.PlayAnimation(mEntityID, dropAnimName);
    currState = State.ANIMATION;
  }
}



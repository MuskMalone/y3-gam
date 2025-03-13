using IGE.Utils;

public class SuspiciousPainting : Entity
{
  // Script to be placed in the SuspiciousPainting Entity (Parent)
  public PlayerInteraction playerInteraction;
  public Entity removePaintingUI;
  public string dropAnimName;
  public float timeUntilDespawn;

  private enum State
  {
    ON_WALL,    // starting state
    ANIMATION,  // animation to tilt painting
    TILTED,     // painting has been tilted
    FALLEN      // after falling to the ground
  }

  private State currState = State.ON_WALL;
  private float timeElapsed = 0.0f;

  void Start()
  {
    removePaintingUI?.SetActive(false);
  }

  void Update()
  {
    switch (currState)
    {
      case State.ON_WALL:
        bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
        if (isPaintingHit && Input.GetMouseButtonTriggered(0))
        {
          InternalCalls.PlayAnimation(InternalCalls.GetParentByID(mEntityID), dropAnimName);
          InternalCalls.PlaySound(mEntityID, "UnhookSound");

          currState = State.ANIMATION;
          removePaintingUI.SetActive(false);
          return;
        }

        removePaintingUI.SetActive(isPaintingHit);
        break;

      case State.ANIMATION:
        // update collider while animation is playing
        InternalCalls.UpdatePhysicsToTransform(mEntityID);

        // switch state when animation ends
        if (!InternalCalls.IsPlayingAnimation(InternalCalls.GetParentByID(mEntityID)))
        {
          currState = State.TILTED;
        }
        break;

      case State.TILTED:
        // unlock the rigidbody
        InternalCalls.LockRigidBody(mEntityID, false);
        InternalCalls.SetGravityFactor(mEntityID, 20.0f);
        currState = State.FALLEN;
        InternalCalls.PlaySound(mEntityID, "DropSound");
        break;

      case State.FALLEN:
        timeElapsed += InternalCalls.GetDeltaTime();

        // if time has exceeded, destroy the entire painting entity
        if (timeElapsed >= timeUntilDespawn)
        {
          InternalCalls.DestroyEntity(InternalCalls.GetParentByID(mEntityID));
        }
        break;
    }
  }
}
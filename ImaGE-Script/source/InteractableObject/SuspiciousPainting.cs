using IGE.Utils;

public class SuspiciousPainting : Entity
{
  // Script to be placed in the SuspiciousPainting Entity (Parent)
  public PlayerInteraction playerInteraction;
  public Entity removePaintingUI;
  public string dropAnimName;
  private bool playerInteracted = false, isPlayingAnim = false;

  void Start()
  {
    removePaintingUI?.SetActive(false);
  }

  void Update()
  {
    if (!playerInteracted)
    {
      bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetMouseButtonTriggered(0) && isPaintingHit)
      {
        InternalCalls.PlayAnimation(InternalCalls.GetParentByID(mEntityID), dropAnimName);

        isPlayingAnim = playerInteracted = true;
        removePaintingUI.SetActive(false);
        return;
      }

      removePaintingUI.SetActive(isPaintingHit);
    }
    else
    {
      // animation to tilt painting
      if (isPlayingAnim)
      {
        // update collider while animation is playing
        InternalCalls.UpdatePhysicsToTransform(mEntityID);

        isPlayingAnim = InternalCalls.IsPlayingAnimation(InternalCalls.GetParentByID(mEntityID));
      }
      // when animation ends, trigger physics to drop the painting
      else
      {
        // unlock the rigidbody
        InternalCalls.LockRigidBody(mEntityID, false);
        InternalCalls.SetGravityFactor(mEntityID, 30.0f);
      }
    }
  }
}
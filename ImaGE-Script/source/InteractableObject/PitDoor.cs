using IGE.Utils;

public class PitDoor : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity gapCollider;
  public Entity unlockDoorUI = null;
  public string openAnimName;

  private string entityTag;
  private bool playerInteracted = false;

  // Start is called before the first frame update
  void Start()
  {
    entityTag = InternalCalls.GetTag(mEntityID);
    gapCollider.SetActive(true);
  }

  // Update is called once per frame
  void Update()
  {
    // if player hasn't interacted with door
    if (!playerInteracted)
    {
      bool isDoorHit = playerInteraction.RayHitString == entityTag;
      if (Input.GetMouseButtonTriggered(0) && isDoorHit)
      {
        GetComponent<Animation>().Play(openAnimName);
        InternalCalls.DestroyEntity(gapCollider.mEntityID);
        playerInteracted = true;
        unlockDoorUI?.SetActive(false);
      }
      else
      {
        unlockDoorUI?.SetActive(isDoorHit);
      }
    }
    // else animation in progress
    else
    {
      // update collider
      InternalCalls.UpdatePhysicsToTransform(mEntityID);

      if (!GetComponent<Animation>().IsPlaying())
      {
        Destroy(this);  // destroy this script
      }
    }
  }
}



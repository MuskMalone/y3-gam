using IGE.Utils;

public class Safe : Entity
{
  // Script to be placed in the Safe Entity (Parent)
  public PlayerInteraction playerInteraction;
  public Entity interactWithSafeUI;

  private bool safeInteraction = false;

  void Start()
  {

  }

  void Update()
  {
    bool isSafeHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetKeyTriggered(0) && isSafeHit)
    {
      safeInteraction = true;
    }
    interactWithSafeUI.SetActive(isSafeHit);
  }
}

using IGE.Utils;

public class SuspiciousPainting : Entity
{
  // Script to be placed in the SuspiciousPainting Entity (Parent)
  public PlayerInteraction playerInteraction;
  public Entity removePaintingUI;

  void Start()
  {
    removePaintingUI?.SetActive(false);
  }

  void Update()
  {
    if (IsActive())
    {
      bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetMouseButtonTriggered(0) && isPaintingHit)
      {
        SetActive(false);
        removePaintingUI.SetActive(false);
        return;
      }

      removePaintingUI.SetActive(isPaintingHit);
    }
  }
}
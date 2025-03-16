using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class PickupCompletedPainting : Entity
{
  public BlackBorder blackBorder;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;

  public PickupCompletedPainting() : base()
  {

  }

  void Start()
  {
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    if (IsActive())
    {
      bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetKeyTriggered(KeyCode.E) && isPaintingHit)
      {
        InternalCalls.PlaySound(mEntityID, "PickupObjects");
        BeginCutscene();
        SetActive(false);
      }
      EToPickUpUI.SetActive(isPaintingHit);
    }
  }

  void BeginCutscene()
  {
    blackBorder.DisplayBlackBorders();
  }
}
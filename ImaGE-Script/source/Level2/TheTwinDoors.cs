using IGE.Utils;
using System;
using System.Numerics;
using System.Text;
using static System.TimeZoneInfo;

public class TheTwinDoors : Entity
{
  public Level2Inventory level2Inventory;
  public PlayerInteraction playerInteraction;
  public Entity leftDoor, rightDoor;
  public string doorAnimName;

  private string entityTag;
  private bool playerInteracted = false;
  public TheTwinDoors() : base()
  {

  }

  void Start()
  {
    entityTag = InternalCalls.GetTag(mEntityID);
  }

  void Update()
  {
    if (!playerInteracted)
    {
      if (Input.GetMouseButtonTriggered(0) && playerInteraction.RayHitString == entityTag)
      {
        level2Inventory.ClearInventory();
        SetActive(false);
        playerInteracted = true;
        InternalCalls.PlayAnimation(leftDoor.mEntityID, doorAnimName);
        InternalCalls.PlayAnimation(rightDoor.mEntityID, doorAnimName);
      }

      return;
    }

    // align collider here
  }
}
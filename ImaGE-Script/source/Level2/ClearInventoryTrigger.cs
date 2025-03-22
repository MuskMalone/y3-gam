using IGE.Utils;

public class ClearInventoryTrigger : Entity
{
  public Entity player;
  public Level2Inventory level2Inventory;

  // Start is called before the first frame update
  void Start() {}

  // Update is called once per frame
  void Update()
  {
    if (InternalCalls.OnTriggerEnter(mEntityID, player.mEntityID))
    {
      level2Inventory.ClearInventoryForHexRoom();
    }
  }
}

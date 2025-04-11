using System.Threading;
using IGE.Utils;

public class ClearInventoryTrigger : Entity
{
  public Entity player;
  public Level2Inventory level2Inventory;

  bool start = false;
  float timer = 0f;
  float volDownTimer = 0.5f;

  // Start is called before the first frame update
  void Start() {}
  // Update is called once per frame
  void Update()
  {
    if (InternalCalls.OnTriggerEnter(mEntityID, player.mEntityID))
    {
      level2Inventory.ClearInventoryForHexRoom();
      start = true;
    }
    if (start) { 
      if (timer <= volDownTimer) {
        timer += InternalCalls.GetDeltaTime();
        InternalCalls.SetSoundVolume(player.mEntityID, "..\\Assets\\Audio\\Picture Perfect - BGM1_Vers1_Loop.wav", Easing.Linear(0.6f, 0f, timer / volDownTimer));
      }

    }
  }
}

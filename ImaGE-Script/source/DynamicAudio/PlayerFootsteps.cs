using System;
using IGE.Utils;
using System.Numerics;

public class PlayerFootsteps : Entity
{
  //public Entity audioManager;
  public Entity player;
  public PlayerMove playerMoveScript;
  public bool isLevel1 = false;
  public Entity doorEntryTrigger;
  public float interval = 0.5f;
  public float footstepVolume = 2f;
  public float footstepPitch = 1f;
  public int footstepSoundCount = 8;
  public float speedThreshold = 0.5f;

  public int ppType = 0;
  public float ppPararm = 1500.0f;
  public string tag = "";
  public bool isGrounded = false;
  // Start is called before the first frame update
  private float timePassed;
  private Random random = new Random();
  private int footstepCount = 0;
  private bool isEnteredHallway;
  public PlayerFootsteps() : base()
  {

  }
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    timePassed += InternalCalls.GetDeltaTime();
    isGrounded = playerMoveScript.IsGrounded();
    Vector3 velocity = InternalCalls.GetVelocity(player.mEntityID);
    float magnitude = velocity.X * velocity.X + velocity.Y * velocity.Y + velocity.Z * velocity.Z;

    Vector3 position = InternalCalls.GetPosition(player.mEntityID);
    //Console.WriteLine($"ISGROUNDED {isGrounded}");
    if (timePassed >= interval)
    {
      if (playerMoveScript.climbing && (InternalCalls.IsKeyHeld(KeyCode.W) || InternalCalls.IsKeyHeld(KeyCode.S)))
      {
        PlayClimbingSound();
      }
      else if (isGrounded && magnitude > speedThreshold)
      {
        PlayFootstepSound();
      }
      timePassed = 0;
    }
    if (isLevel1)
    {
      Vector3 playerLoc = InternalCalls.GetWorldPosition(player.mEntityID);
      Vector3 doorLoc = InternalCalls.GetWorldPosition(doorEntryTrigger.mEntityID);
      isEnteredHallway = playerLoc.X > doorLoc.X;
      if (InternalCalls.OnTriggerExit(doorEntryTrigger.mEntityID, player.mEntityID))
      {
        for (int i = 0; i < footstepSoundCount; ++i)
        {
          string soundName = $"Pavement{i + 1}.wav";

          if (isEnteredHallway)
          {
            InternalCalls.EnableSoundPostProcessing(mEntityID, soundName);//, Convert.ToUInt32(ppType), ppPararm);
          }
          else
          {
            InternalCalls.DisableSoundPostProcessing(mEntityID, soundName);
          }
        }
      }
    }
  }

  public void PlayRandomMetalSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"MetalPipe{footstepCount + 1}";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  public void PlayRandomGrassSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"Grass{footstepCount + 1}.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }

  public void PlayRandomPavementSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"Pavement{footstepCount + 1}.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.SetSoundPitch(mEntityID, soundName, footstepPitch);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }

  public void PlayRandomWoodenSound()
  {
    int soundNumber = random.Next(1, 6);
    string soundName = $"Wooden{soundNumber}";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
  }
  public void PlayRandomWoodenBoardSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"WoodenBoard{footstepCount + 1}_SFX.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  public void PlayRandomSqueakyWoodenBoardSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"WoodenBoard{footstepCount + 1}_SFX.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    soundName = $"Plank{footstepCount + 1}_SFX.wav";
    InternalCalls.DisableSoundPostProcessing(mEntityID, soundName);
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  public void PlayRandomPlankSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"Plank{footstepCount + 1}_SFX.wav";
    InternalCalls.EnableSoundPostProcessing(mEntityID, soundName);//, 0, 1500f);
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  public void PlayRandomGravelSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"Gravel{footstepCount + 1}.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  public void PlayRandomLinoleumSound(bool pp = false)
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"..\\Assets\\Audio\\FootstepLinoleum{footstepCount + 1}_SFX.wav";
    if (pp)
      InternalCalls.EnableSoundPostProcessing(mEntityID, soundName);//, 0, 2000f);
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
    if (pp)
      InternalCalls.DisableSoundPostProcessing(mEntityID, soundName);
  }

  public void PlayRandomCarpetSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"..\\Assets\\Audio\\FootstepCarpet{footstepCount + 1}_SFX.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume * 4f);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  public void PlayRandomEchoeyPavementSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"Pavement{footstepCount + 1}.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.EnableSoundPostProcessing(mEntityID, soundName);//, 0, 2000f);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  void PlayClimbingSound()
  {
    footstepCount = footstepCount % footstepSoundCount;
    string soundName = $"Ladder{footstepCount + 1}_SFX.wav";
    InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
    InternalCalls.PlaySound(mEntityID, soundName);
    footstepCount++;
  }
  void PlayFootstepSound()
  {

    // Raycast downward to detect the ground surface
    Vector3 position = InternalCalls.GetPosition(player.mEntityID);
    Vector3 scale = InternalCalls.GetScale(player.mEntityID);
    uint entityHit = InternalCalls.RaycastFromEntity(player.mEntityID, position, position + (new Vector3(0, -200, 0)));
    tag = "";
    Console.WriteLine($"IMPLAYING {tag}");
    if (entityHit != uint.MaxValue)
    {
      // Check the layer name of the object hit by the raycast
      tag = InternalCalls.GetTag(entityHit);
      //InternalCalls.Log(tag);
      switch (tag)
      {
        case "BumpyGround":
          PlayRandomGrassSound();
          break;
        case "Second Level":
        case "Pit Room Ground":
        case "Pit Room Platform":
        case "Stair Head":
        case "Stair Part":
        case "Altar Platform":
        case "StepCollider":
        case "Crossroads":
          PlayRandomPavementSound();
          break;
          //PlayRandomGravelSound();
          //break;
        case "Floor":
        case "Floor (Main Area)":
        case "Floor (Exit)":
        case "Floor (Room)":
        case "Floor Layer":
          PlayRandomWoodenBoardSound();
          break;
        case "Floor (Pit Room)":
          PlayRandomSqueakyWoodenBoardSound();
          break;
        case "Floorboard1":
        case "Floorboard2":
        case "Floorboard3":
        case "Floorboard7":
          PlayRandomPlankSound();
          break;
        case "Floor2":
        case "Floor2 (Copy)":
          PlayRandomLinoleumSound();
          break;
        case "Floor4":
          PlayRandomLinoleumSound(true);
          break;
        case "BridgeCollider1":
        case "BridgeCollider1 (Copy)":
        case "BottomCollider":
        case "baseCollidert":
        case "MainFloor":
        case "AnimationGroundCollider":
          PlayRandomEchoeyPavementSound();
          break;
        case "Hallway Floor":
        case "Ground":
          PlayRandomCarpetSound();
          break;
        //case "Metal Pipes":
        //    PlayRandomMetalSound();
        //    break;
        case "Wooden Ground":
          PlayRandomWoodenSound();
          break;
        default:
          Debug.Log("No Sound Case " + tag);
          break;
      }
    }
  }
}

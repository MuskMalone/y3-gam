using IGE.Utils;
using System;
using static Dialogue;

public class Hammer : Entity, IInventoryItem
{
  // Script to be placed in the Hammer Entity (Parent)
  public Entity _Image; // Selection UI
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Dialogue dialogueSystem;
  public string[] hammerDialogue;

  public string Name
  {
    get
    {
      return "Hammer";
    }
  }

  public Entity Image
  {
    get
    {
      return _Image;
    }

    set
    {
      _Image = value;
    }
  }


  public void OnPickup()
  {
    SetActive(false);
  }

  public void OnUsed()
  {
    Destroy(mEntityID);
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    bool isHammerHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetKeyTriggered(KeyCode.E) && isHammerHit)
    {
      InternalCalls.PlaySound(mEntityID, "PickupObjects");
      InternalCalls.PlaySound(mEntityID, "IncoherentWhispers");
      inventoryScript.Additem(this);
      dialogueSystem.SetDialogue(hammerDialogue, new Dialogue.Emotion[] { Emotion.Surprised }, 0.006f, "IncoherentWhispers", "..\\Assets\\Textures\\Stagedive-d58X.ttf");
    }
    EToPickUpUI.SetActive(isHammerHit);
  }
}

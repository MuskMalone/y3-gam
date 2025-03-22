using IGE.Utils;
using static Dialogue;

public class CorrdiorPainting : Entity, IInventoryItem
{
  public Entity _Image;
  public Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Dialogue dialogueSystem;
  public string[] pitPaintingDialogue;

  private bool playerCollided = false;
  public string Name
  {
    get
    {
      return "CorridorPainting";
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
    Destroy();
  }

  void Start()
  {
    _Image?.SetActive(false);
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    // For Painting Picking Up
    bool isPaintHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (isPaintHit && Input.GetKeyTriggered(KeyCode.E))
    {
      InternalCalls.PlaySound(mEntityID, "PickupPainting");
      inventoryScript.Additem(this);
      dialogueSystem.SetDialogue(pitPaintingDialogue, new Dialogue.Emotion[] { Emotion.Surprised });  
    }
    EToPickUpUI.SetActive(isPaintHit);

    if (!playerCollided)
    {
      if (InternalCalls.GetContactPoints(mEntityID, playerInteraction.mEntityID).Length > 0)
      {
        InternalCalls.LockRigidBody(mEntityID, true);
        playerCollided = true;
      }
    }
  }
}

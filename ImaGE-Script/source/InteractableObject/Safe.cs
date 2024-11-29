using IGE.Utils;

public class Safe : Entity
{
  // Script to be placed in the Safe Entity (Parent)
  public PlayerInteraction playerInteraction;
  public Entity interactWithSafeUI;
  public Entity safeInstructionsUI;
  public Entity keyPadUI;
  public Entity safeTextBox; // Entity with text component to display the keyed in characters
  public PlayerMove playerMove;

  // Workaround for broken Entity[]
  public Entity ABC;
  public Entity DEF;
  public Entity GHI;
  public Entity JKL;
  public Entity MNO;
  public Entity PQRS;
  public Entity TUV;
  public Entity WXYZ;
  public Entity Enter;

  private bool safeInteraction = false;

  void Start()
  {
    interactWithSafeUI?.SetActive(false);
    keyPadUI?.SetActive(false);
    safeTextBox?.SetActive(false);
    safeInstructionsUI?.SetActive(false);
    ABC?.SetActive(false);
    DEF?.SetActive(false);
    GHI?.SetActive(false);
    JKL?.SetActive(false);
    MNO?.SetActive(false);
    PQRS?.SetActive(false);
    TUV?.SetActive(false);
    WXYZ?.SetActive(false);
    Enter?.SetActive(false);
  }

  void Update()
  {
    if (!safeInteraction)
    {
      bool isSafeHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetKeyTriggered(0) && isSafeHit)
      {
        safeInteraction = true;
        interactWithSafeUI.SetActive(false);
        return;
      }
      interactWithSafeUI.SetActive(isSafeHit);
    }
    
    if (safeInteraction)
    {
      playerMove.FreezePlayer();
      keyPadUI.SetActive(true);
      safeTextBox.SetActive(true);
      safeInstructionsUI.SetActive(true);
    }
  }
}

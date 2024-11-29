using IGE.Utils;
using System.Numerics;

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

  private SafeButtons ABCButtonScript;
  private SafeButtons DEFButtonScript;
  private SafeButtons GHIButtonScript;
  private SafeButtons JKLButtonScript;
  private SafeButtons MNOButtonScript;
  private SafeButtons PQRSButtonScript;
  private SafeButtons TUVButtonScript;
  private SafeButtons WXYZButtonScript;
  private SafeButtons EnterButtonScript;

  private bool safeInteraction = false;
  private bool safeUIActive = false;

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

    ABCButtonScript = ABC.FindObjectOfType<SafeButtons>();
    DEFButtonScript = DEF.FindObjectOfType<SafeButtons>();
    GHIButtonScript = GHI.FindObjectOfType<SafeButtons>();
    JKLButtonScript = JKL.FindObjectOfType<SafeButtons>();
    MNOButtonScript = MNO.FindObjectOfType<SafeButtons>();
    PQRSButtonScript = PQRS.FindObjectOfType<SafeButtons>();
    TUVButtonScript = TUV.FindObjectOfType<SafeButtons>();
    WXYZButtonScript = WXYZ.FindObjectOfType<SafeButtons>();
    EnterButtonScript = Enter.FindObjectOfType<SafeButtons>();
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
        BeginSafeUI();
        return;
      }
      interactWithSafeUI.SetActive(isSafeHit);
    }

    if (safeUIActive)
    {
      SetAllButtonsInactive();

      if (ABCButtonScript.IsVisible) { ABC.SetActive(true); }
      if (DEFButtonScript.IsVisible) { DEF.SetActive(true); }
      if (GHIButtonScript.IsVisible) { GHI.SetActive(true); }
      if (JKLButtonScript.IsVisible) { JKL.SetActive(true); }
      if (MNOButtonScript.IsVisible) { MNO.SetActive(true); }
      if (PQRSButtonScript.IsVisible) { PQRS.SetActive(true); }
      if (TUVButtonScript.IsVisible) { TUV.SetActive(true); }
      if (WXYZButtonScript.IsVisible) { WXYZ.SetActive(true); }
      if (EnterButtonScript.IsVisible) { Enter.SetActive(true); }
    }
  }

  private void BeginSafeUI()
  {
    safeUIActive = true;
    playerMove.FreezePlayer();
    keyPadUI.SetActive(true);
    safeTextBox.SetActive(true);
    safeInstructionsUI.SetActive(true);
  }

  private void EndSafeUI()
  {
    safeUIActive = false;
    playerMove.UnfreezePlayer();
    keyPadUI.SetActive(false);
    safeTextBox.SetActive(false);
    safeInstructionsUI.SetActive(false);
    SetAllButtonsInactive();
  }

  private void SetAllButtonsInactive()
  {
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
}

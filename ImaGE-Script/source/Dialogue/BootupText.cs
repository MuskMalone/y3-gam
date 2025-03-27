using IGE.Utils;
using System.Numerics;

// NEED TEXT COMPONENT AT (-17.1, -7.5, 0) AND "BootUploadingText" SFX
public class BootupText : Entity
{ 
  public PlayerMove playerMove;
  public Transition transition;
  // Boot upload text
  public float fadeDuration = 3f;
  public string introMessage;
  public float typewriterSpeed = 0.04f;

  private bool completed = false;
  private bool triggerInitialFadeIn = true;
  private bool triggerInitialSpecialDialogue = false;
  private string specialLine;
  private bool isInSpecialDialogueMode = false;
  private bool specialLineFadeout = false;
  private float fadeTime = 0f;
  private int charIndex = 0;
  private float nextCharTime = 0f;
  private readonly float specialDialogueFontScale = 0.003f;

  void Start()
  {
    
  }

  void Update()
  {
    if (InternalCalls.GetIsPaused())
    {
      InternalCalls.PauseSound(mEntityID, "BootUploadingText");
      InternalCalls.SetActive(mEntityID, false);
      return;
    }

    if (isInSpecialDialogueMode && !InternalCalls.GetIsPaused())
    {
      // InternalCalls.ResumeSound(mEntityID, "BootUploadingText"); Need resume sound to exist
      InternalCalls.SetActive(mEntityID, true);
      playerMove.FreezePlayer();
    }

    if (triggerInitialFadeIn)
    {
      playerMove.FreezePlayer();
      transition.StartTransition(true, fadeDuration, Transition.TransitionType.TV_SWITCH);
      triggerInitialSpecialDialogue = true;
      triggerInitialFadeIn = false;
    }

    if (triggerInitialSpecialDialogue)
    {
      if (transition.IsFinished())
      {
        SetSpecialDialogue(introMessage, specialDialogueFontScale);
        triggerInitialSpecialDialogue = false;
      }
    }

    if (isInSpecialDialogueMode)
    {
      if (Time.gameTime >= nextCharTime && charIndex < specialLine.Length)
      {
        InternalCalls.AppendText(mEntityID, specialLine[charIndex].ToString());
        charIndex++;
        nextCharTime = Time.gameTime + typewriterSpeed;
      }

      if (charIndex >= specialLine.Length)
      {
        SpecialLineFadeout();
      }
    }
  }

  private void SetSpecialDialogue(string line, float textScale)
  {
    InternalCalls.SetTextScale(mEntityID, textScale);

    specialLine = line;
    StartSpecialDialogue();
  }

  private void StartSpecialDialogue()
  {
    if (playerMove == null)
    {
      Debug.LogError("PlayerMove script not attached to Dialogue script.");
      return;
    }

    if (specialLine == null || specialLine.Length == 0)
    {
      Debug.LogError("Dialogue lines are not set.");
      return;
    }

    InternalCalls.PlaySound(mEntityID, "BootUploadingText");
    InternalCalls.SetText(mEntityID, string.Empty);
    isInSpecialDialogueMode = true;
    SetActive(true);
    charIndex = 0;
    nextCharTime = Time.gameTime;
  }

  private void EndSpecialDialogue()
  {
    //playerMove.UnfreezePlayer();
    SetActive(false);
    isInSpecialDialogueMode = false;
    completed = true;
  }

  private void SpecialLineFadeout()
  {
    // Stop sound before the fadeout
    InternalCalls.StopSound(mEntityID, "BootUploadingText");
    Vector4 originalColor = InternalCalls.GetTextColor(mEntityID);
    if (fadeTime < fadeDuration)
    {
      float alpha = Easing.Linear(1, 0, fadeTime / fadeDuration);
      InternalCalls.SetTextColor(mEntityID, new Vector4(originalColor.X, originalColor.Y, originalColor.Z, alpha));
      fadeTime += Time.deltaTime;
      return;
    }

    InternalCalls.SetTextColor(mEntityID, new Vector4(originalColor.X, originalColor.Y, originalColor.Z, 0));
    EndSpecialDialogue();
  }

  public bool IsBootupTextCompleted()
  {
    return completed;
  }
}
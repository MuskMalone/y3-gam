using IGE.Utils;
using System.Numerics;
using static Dialogue;

public class SpecialDialogue : Entity
{
  public PlayerMove playerMove;
  public string introMessage;
  public string[] initialDialogue;
  public float fadeDuration = 1f;
  public float typewriterSpeed = 0.04f;

  public Dialogue dialogueSystem;
  public Entity[] BeginningSilhouetteSequence;

  // Private Variables
  private bool triggerInitialSpecialDialogue = true;
  private string specialLine;
  private bool triggerInitialDialogue = false;
  private bool isInSpecialDialogueMode = true;
  private bool specialLineFadeout = false;
  private float fadeTime = 0f;

  private int charIndex = 0;            // Tracks the current character index
  private float nextCharTime = 0f;      // Tracks the time for the next character

  // Font Scales
  private readonly float mainDialogueFontScale = 0.007f;
  private readonly float specialDialogueFontScale = 0.003f;

  public SpecialDialogue() : base()
  {

  }

  void Start()
  {
    for (int i = 0; i < BeginningSilhouetteSequence.Length; i++)
    {
      BeginningSilhouetteSequence[i]?.SetActive(false);
    }
  }

  void Update()
  {
    if (triggerInitialSpecialDialogue)
    {
      SetSpecialDialogue(introMessage, specialDialogueFontScale);
      triggerInitialSpecialDialogue = false;
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
        // Line has ended, fadeout
        SpecialLineFadeout();
      }
    }

    if (triggerInitialDialogue)
    {
      dialogueSystem.SetDialogue(initialDialogue, 
        new Dialogue.Emotion[] { Dialogue.Emotion.Neutral, Dialogue.Emotion.Neutral, 
          Dialogue.Emotion.Happy, Dialogue.Emotion.Surprised, Dialogue.Emotion.Happy}, 
        mainDialogueFontScale);
      triggerInitialDialogue = false;
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
    playerMove.FreezePlayer();
    InternalCalls.SetText(mEntityID, string.Empty);
    isInSpecialDialogueMode = true;
    SetActive(true);
    charIndex = 0;                      // Reset character index for typing effect
    nextCharTime = Time.gameTime;       // Start typing immediately
  }

  private void EndSpecialDialogue()
  {
    playerMove.UnfreezePlayer();
    SetActive(false);
    isInSpecialDialogueMode = false;
    triggerInitialDialogue = true;
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
}

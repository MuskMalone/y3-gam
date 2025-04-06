using IGE.Utils;
using System.Numerics;
using System;
using System.Collections.Generic;
using static Dialogue;

public class SpecialDialogue : Entity
{
  public bool skipCutscene;
  public Transition transition;
  public PlayerMove playerMove;
  public string introMessage;
  public string[] initialDialogue;
  public float fadeDuration = 3f;
  public float typewriterSpeed = 0.04f;

  public Dialogue dialogueSystem;
  // Workaround for Entity[] Deserialization not working properly
  public Entity firstSilhouette;
  public Entity secondSilhouette;
  public Entity thirdSilhouette;
  public Entity fourthSilhouette;
  private Entity[] BeginningSilhouetteSequence;

  // Private Variables
  private bool triggerFadeTransition = true;
  private bool isInFadeTransition = false;
  private float fadeTransitionTimer = 0f; 
  private float fadeStartTime;
  private bool isFadingOut = true;
  private float fadeProgress = 0f;

  private bool triggerInitialSpecialDialogue = false;
  private string specialLine;
  private bool triggerInitialDialogue = false;
  private bool isInSpecialDialogueMode = false;
  private bool specialLineFadeout = false;
  private float fadeTime = 0f;

  private bool isInSillouetteSequence = false;
  private int currentSillouetteIndex = -1;
  private float sillouetteElapsedTime = 0f;
  private const float activationInterval = 1.0f;

  private int charIndex = 0;            // Tracks the current character index
  private float nextCharTime = 0f;      // Tracks the time for the next character

  // Font Scales
  private readonly float mainDialogueFontScale = 0.004f;
  private readonly float specialDialogueFontScale = 0.003f;

    private bool isIntroDialogue1Active = false; 
    private bool isIntroDialogue2Active = false;
    private bool hasPlayedLine0Sound = false;
    private bool hasPlayedLine1Sound = false;
    private bool hasPlayedLine2Sound = false;

    public SpecialDialogue() : base()
  {

  }

  void Start()
  {
    if (skipCutscene) { Destroy(); }

    // Workaround
    BeginningSilhouetteSequence = new Entity[4];
    BeginningSilhouetteSequence[0] = firstSilhouette;
    BeginningSilhouetteSequence[1] = secondSilhouette;
    BeginningSilhouetteSequence[2] = thirdSilhouette;
    BeginningSilhouetteSequence[3] = fourthSilhouette;

    for (int i = 0; i < BeginningSilhouetteSequence.Length; i++)
    {
      BeginningSilhouetteSequence[i]?.SetActive(false);
    }
  }

  void Update()
  {
    if (InternalCalls.GetIsPaused())
    {
      if (isInSpecialDialogueMode)
      {
        InternalCalls.PauseSound(mEntityID, "BootUploadingText");
        InternalCalls.SetActive(mEntityID, false);
      }

      return;
    }

    if ((isInSpecialDialogueMode || isInSillouetteSequence) && !InternalCalls.GetIsPaused())
    {
      // InternalCalls.ResumeSound(mEntityID, "BootUploadingText"); Need resume sound to exist
      InternalCalls.SetActive(mEntityID, true);
      playerMove.FreezePlayer();
    }

    if (triggerFadeTransition)
    {
      StartFade();
    }

    if (isInFadeTransition)
    {
      float elapsed = Time.gameTime - fadeStartTime;
      fadeTransitionTimer = elapsed;

      //float alpha = Mathf.Lerp(1f, 0f, fadeTransitionTimer / fadeDuration);
      //InternalCalls.SetImageColor(fadeImage.mEntityID, new Vector4(1, 1, 1, alpha));

      if (transition.IsFinished())
      {
        EndFade();
      }
    }

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

    if (isInSillouetteSequence)
    {
      sillouetteElapsedTime += Time.deltaTime;

      // Handle the first sprite initialization
      if (currentSillouetteIndex == -1)
      {
        currentSillouetteIndex = 0;
        Entity firstSprite = BeginningSilhouetteSequence[currentSillouetteIndex];
        firstSprite.SetActive(true);

        // Initialize fade-in for the first sprite
        fadeProgress = 0f;
        Vector4 initialColor = InternalCalls.GetSprite2DColor(firstSprite.mEntityID);
        InternalCalls.SetSprite2DColor(firstSprite.mEntityID,
            new Vector4(initialColor.X, initialColor.Y, initialColor.Z, 0f));

        isFadingOut = true;

        Debug.Log($"Initialized First Sprite: {firstSprite.mEntityID}");
      }

      // Handle fading out of the current sprite
      if (currentSillouetteIndex >= 0 && currentSillouetteIndex < BeginningSilhouetteSequence.Length)
      {
        Entity currentSprite = BeginningSilhouetteSequence[currentSillouetteIndex];

        if (isFadingOut)
        {
          fadeProgress += Time.deltaTime / fadeDuration;

          // Retrieve and modify the color with updated alpha
          Vector4 initialColor = InternalCalls.GetSprite2DColor(currentSprite.mEntityID);
          float newAlpha = 1f - Mathf.Clamp01(fadeProgress);
          InternalCalls.SetSprite2DColor(currentSprite.mEntityID,
              new Vector4(initialColor.X, initialColor.Y, initialColor.Z, newAlpha));

          Debug.Log($"Fading Out - Sprite: {currentSprite.mEntityID}, Alpha: {newAlpha}");

          if (fadeProgress >= 1f)
          {
            currentSprite.SetActive(false);
            fadeProgress = 0f;
            isFadingOut = false;
          }
        }
      }

      // Handle activation and fading in of the next sprite
      if (!isFadingOut && sillouetteElapsedTime >= activationInterval)
      {
        sillouetteElapsedTime = 0f;
        currentSillouetteIndex++;

        if (currentSillouetteIndex < BeginningSilhouetteSequence.Length)
        {
          Entity nextSprite = BeginningSilhouetteSequence[currentSillouetteIndex];
          nextSprite.SetActive(true);

          // Initialize fade-in alpha
          fadeProgress = 0f;
          Vector4 initialColor = InternalCalls.GetSprite2DColor(nextSprite.mEntityID);
          InternalCalls.SetSprite2DColor(nextSprite.mEntityID,
              new Vector4(initialColor.X, initialColor.Y, initialColor.Z, 0f));

          isFadingOut = true;

          Debug.Log($"Activated Sprite: {nextSprite.mEntityID}");
        }
        else
        {
          EndSiloutetteSequence();
        }
      }
    }

    if (triggerInitialDialogue)
    {
      dialogueSystem.SetDialogue(initialDialogue, 
        new Dialogue.Emotion[] { Dialogue.Emotion.Neutral, Dialogue.Emotion.Happy }, 
        mainDialogueFontScale);
            isIntroDialogue1Active = true;
      triggerInitialDialogue = false;
    }

    if(isIntroDialogue1Active)
    {
        if (dialogueSystem.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
        {
            InternalCalls.PlaySound(mEntityID, "L1_1_VER1");
            hasPlayedLine0Sound = true;
        }
        else if (dialogueSystem.CurrentLineIndex == 1 && !hasPlayedLine1Sound)
        {
            InternalCalls.StopSound(mEntityID, "L1_1_VER1");
            InternalCalls.PlaySound(mEntityID, "L1_2_VER1");
            hasPlayedLine1Sound = true;
            // Optionally disable the dialogue active flag if there are no more sounds
            isIntroDialogue1Active = false;
        }
    }

    if (isIntroDialogue2Active)
    {
        if (dialogueSystem.CurrentLineIndex == 0 && !hasPlayedLine2Sound)
        {
            InternalCalls.StopSound(mEntityID, "L1_2_VER1");
            InternalCalls.PlaySound(mEntityID, "L1_3_VER2");
            hasPlayedLine2Sound = true;
            //isIntroDialogue2Active = false;
        }
    }

        if (isIntroDialogue2Active && !dialogueSystem.isInDialogueMode)
        {
            // Dialogue has finished, so it’s now safe to destroy this entity.
            isIntroDialogue2Active = false;
            Destroy();
        }
    }

  private void StartFade()
  {
    transition.StartTransition(true, fadeDuration, Transition.TransitionType.TV_SWITCH);

    triggerFadeTransition = false;
    isInFadeTransition = true;
    playerMove.FreezePlayer();

    isInFadeTransition = true;
    fadeStartTime = Time.gameTime;
  }
  private void EndFade()
  {
    playerMove.UnfreezePlayer();
    isInFadeTransition = false;
    triggerInitialSpecialDialogue = true;
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

  public void StartSilhouetteSequence()
  {
    isInSillouetteSequence = true;
    playerMove.FreezePlayer();
  }

  private void EndSiloutetteSequence()
  {
    playerMove.UnfreezePlayer();
    isInSillouetteSequence = false;

    dialogueSystem.SetDialogue(new string[] { "She left something on that table..."},
        new Dialogue.Emotion[] { Dialogue.Emotion.Surprised },
        mainDialogueFontScale);
        isIntroDialogue2Active = true;
    InternalCalls.SetShaderState(0, false);
    //Destroy();
  }
}

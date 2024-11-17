using IGE.Utils;
using System;
using System.Numerics;
using System.Reflection;

public class Dialogue : Entity
{
  // NOTE: THE DIALOGUE SCRIPT SHOULD BE ATTACHED TO ENTITY WITH TEXT COMPONENT

  // Public variables
  public enum Emotion
  {
      Neutral,
      Thinking,
      Shocked,
      Surprised,
      Happy,
      Disturbed,
      Sad
  }

  public float fadeDuration = 0.5f;     // Duration for the fade-out effect
  public float typewriterSpeed = 0.3f;  // Speed of the typewriter effect (time between each letter)
  public bool isInDialogueMode;

  public string[] initialDialogue;

  public PlayerMove playerMove;
  public Entity DialogueBox;
  public Entity NeutralTara;
  public Entity ThinkingTara;
  public Entity ShockedTara;
  public Entity SurprisedTara;
  public Entity HappyTara;
  public Entity DisturbedTara;
  public Entity SadTara;

  // Private Variables
  private Emotion[] emotions;
  private int lineIndex = 0;
  private string[] lines;
  private int charIndex = 0;            // Tracks the current character index
  private float nextCharTime = 0f;      // Tracks the time for the next character

  public Dialogue() : base()
  {
   
  }

  void Start()
  {
    if (playerMove == null) Debug.LogError("[Dialogue.cs] PlayerMove Script Entity not found!");
    DeactivateAllEmotions();
  }

  void Update()
  {
    Debug.Log(Time.GameTime.ToString());
    /*
    if (isInDialogueMode && IsActive() && Time.time >= nextCharTime && charIndex < lines[index].Length)
    {
      if (charIndex == 0)
      {
        PlayDialogueSound();
      }

      textComponent.text += lines[index][charIndex];
      charIndex++;
      nextCharTime = Time.time + textSpeed;

      if (charIndex >= lines[index].Length)
      {
        audioManager.StopDialogueSound();
      }
    }

    if (isInDialogueMode && IsActive() && Input.GetMouseButtonDown(0))
    {
      if (charIndex >= lines[index].Length)
      {
        NextLine();
      }
      else
      {
        SkipTyping();
      }
    }
    */
  }

  void SetEmotion(Emotion emotion)
  {
    DeactivateAllEmotions();

    switch (emotion)
    {
      case Emotion.Thinking:
        ThinkingTara.SetActive(true);
        break;
      case Emotion.Shocked:
        ShockedTara.SetActive(true);
        break;
      case Emotion.Surprised:
        SurprisedTara.SetActive(true);
        break;
      case Emotion.Happy:
        HappyTara.SetActive(true);
        break;
      case Emotion.Disturbed:
        DisturbedTara.SetActive(true);
        break;
      case Emotion.Sad:
        SadTara.SetActive(true);
        break;
      case Emotion.Neutral:
        NeutralTara.SetActive(true);
        break;
    }
  }

  void DeactivateAllEmotions()
  {
    ThinkingTara.SetActive(false);
    ShockedTara.SetActive(false);
    SurprisedTara.SetActive(false);
    HappyTara.SetActive(false);
    DisturbedTara.SetActive(false);
    SadTara.SetActive(false);
    NeutralTara.SetActive(false);
  }

  void NextLine()
  {
    DeactivateAllEmotions();

    if (lineIndex < lines.Length - 1)
    {
      lineIndex++;
      SetEmotion(emotions[lineIndex]);
      InternalCalls.SetText(mEntityID, string.Empty);
      charIndex = 0;                                            // Reset character index for new line
      nextCharTime = Time.GameTime;                             // Start typing new line immediately
    }
    else
    {
      EndDialogue();
    }
  }

  void EndDialogue()
  {
    UnfreezePlayer();
    SetActive(false);
    isInDialogueMode = false;
  }

  public void FreezePlayer()
  {
    if (playerMove != null)
    {
      playerMove.canMove = false;
      playerMove.canLook = false;
    }
  }

  public void UnfreezePlayer()
  {
    if (playerMove != null)
    {
      playerMove.canMove = true;
      playerMove.canLook = true;
    }
  }
}
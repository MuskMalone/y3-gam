using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class InitialSequence : Entity
{
  public PlayerMove playerMove;
  public Transition transition;
  public Lvl4Dialogue dialogue;
  public string[] initialText;
  public float fadeDuration = 3f;

  private bool triggerFadeTransition = true;
  private bool isInFadeTransition = false;
  private float fadeTransitionTimer = 0f;
  private float fadeStartTime;
  private bool isFadingOut = true;
  private float fadeProgress = 0f;
  private bool triggerInitialSpecialDialogue = false;

    private bool isDialogueActive = false;
    private bool hasPlayedLine0Sound = false;

    public InitialSequence() : base()
  {

  }

  void Start()
  {
    
  }

  void Update()
  {
    if (triggerFadeTransition)
    {
      StartFade();
    }

    if (isInFadeTransition)
    {
      float elapsed = Time.gameTime - fadeStartTime;
      fadeTransitionTimer = elapsed;

      if (transition.IsFinished())
      {
        EndFade();
      }
    }

    if (triggerInitialSpecialDialogue)
    {
      dialogue.SetDialogue(initialText, new Lvl4Dialogue.Emotion[] { Lvl4Dialogue.Emotion.Happy});
            isDialogueActive = true;
      triggerInitialSpecialDialogue = false;
    }

        if (isDialogueActive)
        {
            if (dialogue.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L4_1");
                hasPlayedLine0Sound = true;
                isDialogueActive = false;
            }
            
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
}
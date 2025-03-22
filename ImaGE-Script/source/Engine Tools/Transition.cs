using IGE.Utils;
using System;
using static Transition;

// This script is to be placed in the Canvas entity
public class Transition : Entity
{
  public enum TransitionType : int
  {
    FADE = 0,         // Smooth fade in/out
    TV_SWITCH = 1,    // TV switch on/off effect
    WIPE = 2          // Screen wipe transition
  };


  private bool isFadeIn;
  private float totalTime;
  private float transitionTimer = 0f;
  private bool finished = false;
  private float transitionProgress = 0f;
  private TransitionType transitionTypeInternal;

  private enum State
  {
    NONE,
    NORMAL,
    INOUT,
    OUTIN
  }
  private State currState = State.NONE;
  private State selectedState = State.NONE;

  void Start()
  {
    InternalCalls.EnableCanvasTransition(mEntityID, true); // Always enable transition
  }

  void Update()
  {
    if (!finished)
    {
      transitionTimer += Time.deltaTime;
      float progress = Mathf.Clamp01(transitionTimer / totalTime);

      // If it's a fade-in, invert the progress
      transitionProgress = Mathf.Clamp(isFadeIn ? 1f - progress : progress, 0f, 1f);

      InternalCalls.SetCanvasTransitionProgress(mEntityID, transitionProgress);

      if (progress >= 1f)
      {
        finished = true;
      }

      if (finished && selectedState == State.INOUT)
      {
        StartTransition(false, totalTime, transitionTypeInternal);
        selectedState = State.NONE;
      }

      if (finished && selectedState == State.OUTIN)
      {
        StartTransition(true, totalTime, transitionTypeInternal);
        selectedState = State.NONE;
      }
    }
  }

  // These functions are to be called from other scripts
  public void StartTransition(bool fadeIn, float duration, TransitionType transitionType)
  {
    Debug.Log("Starting Transition");
    isFadeIn = fadeIn;
    totalTime = duration;
    transitionTimer = 0f;
    finished = false;
    transitionProgress = fadeIn ? 1f : 0f;                 // Initialize based on fade type
    InternalCalls.EnableCanvasTransition(mEntityID, true); // Ensure transition is enabled
    InternalCalls.SetCanvasTransitionProgress(mEntityID, transitionProgress);
    transitionTypeInternal = transitionType;
    InternalCalls.SetCanvasTransitionType(mEntityID, (int)transitionTypeInternal);
  }

  public void StartTransitionInOut(float duration, TransitionType transitionType)
  {
    selectedState = State.INOUT;
    StartTransition(true, duration, transitionType);
  }

  public void StartTransitionOutIn(float duration, TransitionType transitionType)
  {
    selectedState = State.OUTIN;
    StartTransition(false, duration, transitionType);
  }

  public bool IsFinished()
  {
    return finished;
  }

  public float GetProgress()
  {
    return transitionProgress;
  }
}
using IGE.Utils;
using System;

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
      transitionProgress = isFadeIn ? 1f - progress : progress;

      InternalCalls.SetCanvasTransitionProgress(mEntityID, transitionProgress);

      if (progress >= 1f)
      {
        finished = true;
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
    InternalCalls.SetCanvasTransitionType(mEntityID, (int)transitionType);
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
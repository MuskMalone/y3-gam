using IGE.Utils;

public class Level3ExitTrigger : Entity
{
  public Level3Dialogue dialogueSystem;
  public string[] leavingWithoutFragmentDialogue;
  public Fragment fragment;
  public float transitionTime = 3f;
  public Transition transition;
  public BlackBorder blackBorder;
  public string turnAroundAnimationName; // Place the animation in player

  private float transitionTimer = 0f;
  private bool animationPlaying = false;

  private enum State
  {
    NONE,
    IN_INTERACTION, // Triggered
    IN_ANIMATION,   // Turning around
    TO_NEXT_LEVEL   // Going to next level
  }
  private State currState = State.NONE;

  void Start()
  {

  }

  void Update()
  {
    switch (currState)
    {
      case State.NONE:
        {
          if (InternalCalls.OnTriggerEnter(mEntityID, blackBorder.playerMove.mEntityID)) {
            if (!fragment.IsFragmentCollected())
            {
              dialogueSystem.SetDialogue(leavingWithoutFragmentDialogue,
                new Level3Dialogue.Emotion[] { Level3Dialogue.Emotion.Thinking },
                0.006f);
              currState = State.IN_INTERACTION;
            }

            else 
            {
              currState = State.TO_NEXT_LEVEL;
              transition.StartTransition(false, transitionTime, Transition.TransitionType.TV_SWITCH);
            }
          }

          break;
        }
      
      case State.IN_INTERACTION:
        {
          // Dialogue ended
          if (!dialogueSystem.isInDialogueMode)
          {
            transition.StartTransition(false, transitionTime, Transition.TransitionType.FADE);
            blackBorder.DisplayBlackBorders();
            blackBorder.playerMove.useScriptRotation = false;
            currState = State.IN_ANIMATION;
          }
          break;
        }

      case State.IN_ANIMATION:
        {
          transitionTimer += Time.deltaTime;

          if (transitionTimer > transitionTime && animationPlaying == false)
          {
            animationPlaying = true;
            
            InternalCalls.PlayAnimation(blackBorder.playerMove.mEntityID, turnAroundAnimationName);
            InternalCalls.UpdatePhysicsToTransform(blackBorder.playerMove.mEntityID);
            blackBorder.playerMove.useScriptRotation = false;
          }

          if (animationPlaying)
          {
            if (!InternalCalls.IsPlayingAnimation(blackBorder.playerMove.mEntityID))
            {
              blackBorder.playerMove.useScriptRotation = true;
              InternalCalls.UpdatePhysicsToTransform(blackBorder.playerMove.mEntityID);
              blackBorder.HideBlackBorders();
              Reset();
            }
          }

          break;
        }
       
      case State.TO_NEXT_LEVEL:
        {
          transitionTimer += Time.deltaTime;
          if (transitionTimer > transitionTime)
          {
            Reset();
            InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\Level4.scn");
          }
          break;
        }
    }
  }

  void Reset()
  {
    animationPlaying = false;
    currState = State.NONE;
    transitionTimer = 0f;
  }
}
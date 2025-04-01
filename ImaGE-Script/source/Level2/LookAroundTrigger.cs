using IGE.Utils;
using static HexTableOrb;
using static System.TimeZoneInfo;
using System.Numerics;

// Place in the lookaroundtrigger entity
public class LookAroundTrigger : Entity
{
  public Entity playerCamera;
  public Entity lookAroundCamera;
  public Transition transition;
  public BlackBorder blackBorder;
  public string lookAroundAnimationName; // Place the animation in lookAroundCamera
  public float transitionTime = 1f;

  private float transitionTimer = 0f;
  private bool animationPlaying = false;

  private enum State
  {
    NONE,
    IN_ANIMATION,
    FINISHED
  }
  private State currState = State.NONE;

  void Start() { }

  void Update()
  {
    switch (currState)
    {
      case State.NONE:
        {
          if (InternalCalls.OnTriggerEnter(mEntityID, blackBorder.playerMove.mEntityID))
          {
            currState = State.IN_ANIMATION;
            transition.StartTransition(true, transitionTime, Transition.TransitionType.FADE);
            blackBorder.DisplayBlackBorders();
          }

          break;
        }

      case State.IN_ANIMATION:
        {
          transitionTimer += Time.deltaTime;

          if (transitionTimer > transitionTime && !animationPlaying)
          {
            SetLookAroundCameraAsMain();
            transition.StartTransition(false, transitionTime, Transition.TransitionType.FADE);
            lookAroundCamera.GetComponent<Animation>().Play(lookAroundAnimationName);
            animationPlaying = true;
          }

          if (animationPlaying)
          {
            if (!lookAroundCamera.GetComponent<Animation>().IsPlaying())
            {
              transition.StartTransition(false, 0.5f, Transition.TransitionType.FADE);
              blackBorder.HideBlackBorders();
              currState = State.FINISHED;
              SetPlayerCameraAsMain();
              SetActive(false);
            }
          }
          break;
        }

      case State.FINISHED:
        {
          break;
        }
    }
  }

  private void SetLookAroundCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "PlayerCamera");
    InternalCalls.SetTag(lookAroundCamera.mEntityID, "MainCamera");
  }

  private void SetPlayerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(lookAroundCamera.mEntityID, "LookAroundCamera");
  }
}
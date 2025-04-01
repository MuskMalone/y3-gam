using IGE.Utils;
using System;
using System.Data;
using System.Numerics;
using static System.TimeZoneInfo;

public class Fragment : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity fragmentPickupUI;
  public string fragAnimNameFlyUp;
  public string fragAnimNameEnterBox;
  public Entity altarArea;  //leave blank if not needed, just to only set active during animation
  public Entity playerCamera, fragmentCamera;
  public Entity GardenFragment, Particles;
  public Entity ParticleBurst;  // boom
  public Transition transition;
  public BlackBorder blackBorder;
  private bool fragmentCollected = false;

  public Vector3 startPos;
  public Vector3 downPos;
  public Vector3 zoomPos;
  public float explosionWaitTime;

  float elapsedTime = 0.0f;
  float transitionTimeFirstPhase = 4f;
  float transitionTimeSecondPhase = 2.8f;

  private enum State
  {
    IDLE,           // floating in place
    FLY_UP,         // after player interaction
    PANNING_DOWN,   // fragment coming down
    TRANSITION,
    ZOOMING_IN,     // fragment fitting into place
    EXPLOSION,      // boom
    FINAL_PHASE,    // animation done
    COLLECTED
  }
  private State currState = State.IDLE;

  void Start()
  {
    fragmentPickupUI?.SetActive(false);
    SetAltarActive(false);
  }

  void Update()
  {
    if (Input.GetKeyTriggered(KeyCode.RIGHT_BRACKET))
    {
      currState = State.COLLECTED;
      fragmentCollected = true;
    }

    switch (currState)
    {
      // check for player interaction
      case State.IDLE:
        {
          // To place script on fragment collider
          bool isFragmentHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
          if (Input.GetKeyTriggered(KeyCode.E) && isFragmentHit)
          {
            TriggerAnimation();
            currState = State.FLY_UP;
            fragmentPickupUI.SetActive(false);
            fragmentCollected = true;
          }
          else
          {
            fragmentPickupUI.SetActive(isFragmentHit);
          }

          break;
        }
      case State.FLY_UP:
        {
          if (InternalCalls.IsPlayingAnimation(mEntityID)) { return; }

          SetPlayerCameraAsMain();
          transition.StartTransition(false, 2f, Transition.TransitionType.WIPE);
          currState = State.TRANSITION;

          break;
        }
      case State.TRANSITION:
        {
          if (!transition.IsFinished()) { return; }

          TriggerFlyinAnimation();
          currState = State.PANNING_DOWN;

          break;
        }
      // First Phase: Going Down
      case State.PANNING_DOWN:
        {
          elapsedTime += Time.deltaTime;

          Vector3 newPos = Vector3.Lerp(startPos, downPos, elapsedTime / transitionTimeFirstPhase);
          InternalCalls.SetPosition(fragmentCamera.mEntityID, ref newPos);

          if (elapsedTime >= transitionTimeFirstPhase)
          {
            elapsedTime = 0f;   // Reset timer
            currState = State.ZOOMING_IN; // Switch to next phase
          }

          break;
        }
      // Second Phase: Zoom In
      case State.ZOOMING_IN:
        {
          elapsedTime += Time.deltaTime;

          Vector3 newPos = Vector3.Lerp(downPos, zoomPos, elapsedTime / transitionTimeSecondPhase);
          InternalCalls.SetPosition(fragmentCamera.mEntityID, ref newPos);

          if (elapsedTime >= transitionTimeSecondPhase)
          {
            elapsedTime = 0f;
            ParticleBurst.SetActive(true);  // enable the entity with the emitter
            currState = State.EXPLOSION;
            Particles.SetActive(false);
          }

          break;
        }
      case State.EXPLOSION:
        {
          if (!transition.IsFinished()) { return; }

          elapsedTime += Time.deltaTime;

          if (elapsedTime >= explosionWaitTime)
          {
            elapsedTime = 0f;
            transition.StartTransition(false, 2f, Transition.TransitionType.WIPE);
            currState = State.FINAL_PHASE;
          }

          break;
        }
      case State.FINAL_PHASE:
        {
          if (!transition.IsFinished()) { return; }

          transition.StartTransition(true, 0.5f, Transition.TransitionType.WIPE);
          SetPlayerCameraAsMain();
          blackBorder.HideBlackBorders();
          SetActive(false);
          currState = State.COLLECTED;
          SetAltarActive(false);

          break;
        }
      case State.COLLECTED:
        return;
      default:
        break;
    }
  }

  private void SetFragmentCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "PlayerCamera");
    InternalCalls.SetTag(fragmentCamera.mEntityID, "MainCamera");
  }

  private void SetPlayerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(fragmentCamera.mEntityID, "FragmentCamera");
  }

  void SetAltarActive(bool active)
  {
    if (altarArea.IsValid())
    {
      altarArea.SetActive(active);
    }
  }

  void TriggerAnimation()
  {
    SetAltarActive(true);
    GetComponent<Animation>().Play(fragAnimNameFlyUp);
    InternalCalls.PlaySound(mEntityID, "FloatUpSFX");
    GardenFragment.SetActive(true);
    Particles.SetActive(true);
    blackBorder.DisplayBlackBorders();
  }

  void TriggerFlyinAnimation()
  {
    InternalCalls.PlaySound(GardenFragment.mEntityID, "FlyInSFX");
    transition.StartTransition(true, 0.5f, Transition.TransitionType.WIPE);
    uint parent = InternalCalls.GetParentByID(GardenFragment.mEntityID);
    InternalCalls.PlayAnimation(parent, fragAnimNameEnterBox);
    //Debug.Log("Playing animation: " + fragAnimNameEnterBox);
    SetFragmentCameraAsMain();
  }

  public bool IsFragmentCollected()
  {
    return fragmentCollected;
  }
}

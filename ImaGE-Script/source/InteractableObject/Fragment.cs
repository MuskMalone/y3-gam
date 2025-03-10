using IGE.Utils;
using System;
using System.Numerics;
using static System.TimeZoneInfo;

public class Fragment : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity fragmentPickupUI;
  public string fragAnimNameFlyUp;
  public string fragAnimNameEnterBox;
  public PlayerMove playerMove;
  public Entity playerCamera;
  public Entity fragmentCamera;
  public Entity GardenFragment;
  public Entity Particles;
  public Transition transition;
  public Entity BlackBorder1;
  public Entity BlackBorder2;

  public Vector3 startPos;
  public Vector3 downPos;
  public Vector3 zoomPos;

  float elapsedTime = 0.0f;
  float transitionTimeFirstPhase = 3.0f;
  float transitionTimeSecondPhase = 5.0f;
  bool initialAnimation = true;

  private bool fragmentInteraction = true;
  private Vector3 zoomInPos;
  private Vector3 zoomOutPos;
  bool isZoomingOut = true;
  private string currentAnim = null;
  private bool triggerSecondAnimation = false;
  private bool firstPhase = false;
  private bool finalPhase = false;

  void Start()
  {
    fragmentPickupUI?.SetActive(false);
    BlackBorder1.SetActive(false);
    BlackBorder2.SetActive(false);
  }

  void Update()
  {
    if (fragmentInteraction)
    {
      // To place script on fragment collider
      bool isFragmentHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
      if (Input.GetKeyTriggered(KeyCode.E) && isFragmentHit)
      {
        TriggerAnimation();
        fragmentInteraction = false;
        isFragmentHit = false;
      }
      fragmentPickupUI.SetActive(isFragmentHit);
    }

    // if an animation is in progress
    if (!string.IsNullOrEmpty(currentAnim))
    {
      if (currentAnim == fragAnimNameFlyUp)
      {
        if (!InternalCalls.IsPlayingAnimation(mEntityID))
        {
          currentAnim = null;
          initialAnimation = true;
          SetPlayerCameraAsMain();

          transition.StartTransition(false, 2, Transition.TransitionType.WIPE);
        }
      }

      if (currentAnim == fragAnimNameEnterBox)
      {
        elapsedTime += Time.deltaTime;

        // First Phase: Going Down
        if (firstPhase)
        {
          Vector3 newPos = Vector3.Lerp(startPos, downPos, elapsedTime / transitionTimeFirstPhase);
          InternalCalls.SetPosition(fragmentCamera.mEntityID, ref newPos);

          if (elapsedTime >= transitionTimeFirstPhase)
          {
            firstPhase = false; // Switch to next phase
            elapsedTime = 0f;   // Reset timer
          }
        }

        // Second Phase: Zoom In
        else
        {
          Vector3 newPos = Vector3.Lerp(downPos, zoomPos, elapsedTime / transitionTimeSecondPhase);
          InternalCalls.SetPosition(fragmentCamera.mEntityID, ref newPos);

          if (elapsedTime >= transitionTimeSecondPhase)
          {
            finalPhase = true;
            elapsedTime = 0f;
            transition.StartTransition(false, 2, Transition.TransitionType.WIPE);
          }
        }
      }
    }

    if (string.IsNullOrEmpty(currentAnim) && triggerSecondAnimation && transition.IsFinished())
    {
      triggerSecondAnimation = false;
      TriggerFlyinAnimation();
    }

    if (finalPhase)
    {
      transition.StartTransition(true, 0.5f, Transition.TransitionType.WIPE);
      SetPlayerCameraAsMain();
      playerMove.UnfreezePlayer();
      finalPhase = false;
      currentAnim = null;
      Particles.SetActive(false);
      Destroy(mEntityID);
      BlackBorder1.SetActive(false);
      BlackBorder2.SetActive(false);
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

  void TriggerAnimation()
  {
    currentAnim = fragAnimNameFlyUp;
    InternalCalls.PlayAnimation(mEntityID, fragAnimNameFlyUp, false);
    playerMove.FreezePlayer();
    triggerSecondAnimation = true;
    GardenFragment.SetActive(true);
    Particles.SetActive(true);
    BlackBorder1.SetActive(true);
    BlackBorder2.SetActive(true);
  }

  void TriggerFlyinAnimation()
  {
    firstPhase = true;
    SetActive(false);
    transition.StartTransition(true, 0.5f, Transition.TransitionType.WIPE);
    uint parent = InternalCalls.GetParentByID(GardenFragment.mEntityID);
    InternalCalls.PlayAnimation(parent, fragAnimNameEnterBox, false);
    currentAnim = fragAnimNameEnterBox;
    SetFragmentCameraAsMain();
  }
}

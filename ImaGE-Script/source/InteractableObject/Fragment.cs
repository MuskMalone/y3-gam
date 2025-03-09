using IGE.Utils;
using System;
using System.Numerics;

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
  public Transition transition;

  float elapsedTime = 0.0f;
  float zoomOutDuration = 2.50f;
  bool initialAnimation = true;

  private bool fragmentInteraction = true;
  private Vector3 zoomInPos;
  private Vector3 zoomOutPos;
  bool isZoomingOut = true;
  private string currentAnim = null;
  private bool triggerSecondAnimation = false;

  void Start()
  {
    fragmentPickupUI?.SetActive(false);
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
          Console.WriteLine("End");
          Debug.Log("End");
          currentAnim = null;
          initialAnimation = true;
          SetPlayerCameraAsMain();

          transition.StartTransition(false, 2, Transition.TransitionType.WIPE);
        }
      }

      if (currentAnim == fragAnimNameEnterBox)
      {

      }
    }

    if (string.IsNullOrEmpty(currentAnim) && triggerSecondAnimation && transition.IsFinished())
    {
      transition.StartTransition(true, 0.5f, Transition.TransitionType.WIPE);
      uint parent = InternalCalls.GetParentByID(GardenFragment.mEntityID);
      InternalCalls.PlayAnimation(parent, fragAnimNameEnterBox, false);
      currentAnim = fragAnimNameEnterBox;
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
  }
}

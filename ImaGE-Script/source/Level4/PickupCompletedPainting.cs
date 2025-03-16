using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class PickupCompletedPainting : Entity
{
  public BlackBorder blackBorder;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Entity playerCamera, gateCamera;
  public Entity leftGate;
  public Entity gateBloom;
  public Entity exitTrigger;
  public string leftGateAnimationName;
  public float maximumBloom = 20f;
  public Lvl4Dialogue dialogue;

  private float currentBloomIntensity = 0f;
  private float bloomProgress = 0f;
  private float lerpDuration = 6f;
  private float elapsedTime = 0f;

  private enum State
  {
    NONE,
    CUTSCENE,
    CUTSCENE_END
  }
  private State currState = State.NONE;

  public PickupCompletedPainting() : base()
  {

  }

  void Start()
  {
    exitTrigger.SetActive(false);
    EToPickUpUI?.SetActive(false);
  }

  void Update()
  {
    switch (currState)
    {
      case State.NONE:
        {
          bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
          if (Input.GetKeyTriggered(KeyCode.E) && isPaintingHit)
          {
            InternalCalls.PlaySound(mEntityID, "PickupObjects");
            BeginCutscene();
            isPaintingHit = false;
            SetActive(false);
          }
          EToPickUpUI.SetActive(isPaintingHit);
          break;
        }

      case State.CUTSCENE:
        {
          if (InternalCalls.IsPlayingAnimation(leftGate.mEntityID)) {
            elapsedTime += Time.deltaTime;
            bloomProgress = Mathf.Clamp01(elapsedTime / lerpDuration);
            currentBloomIntensity = Easing.EaseInBounce(0f, maximumBloom, bloomProgress);
            InternalCalls.SetBloomIntensity(gateBloom.mEntityID, currentBloomIntensity);
            return; 
          }

          EndCutscene();

          break;
        }

      case State.CUTSCENE_END:
        {

          break;
        }
    }
  }

  void BeginCutscene()
  {
    blackBorder.DisplayBlackBorders();
    currState = State.CUTSCENE;
    SetGateCameraAsMain();
    InternalCalls.PlayAnimation(leftGate.mEntityID, leftGateAnimationName);
  }

  void EndCutscene()
  {
    blackBorder.HideBlackBorders();
    SetPlayerCameraAsMain();
    currState = State.CUTSCENE_END;
    exitTrigger.SetActive(true);
    dialogue.SetDialogue(new string[] { "Now that the painting is completed, I just\nneed to enter this gate." }, 
      new Lvl4Dialogue.Emotion[] { Lvl4Dialogue.Emotion.Thinking });
  }

  private void SetGateCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "PlayerCamera");
    InternalCalls.SetTag(gateCamera.mEntityID, "MainCamera");
  }

  private void SetPlayerCameraAsMain()
  {
    InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
    InternalCalls.SetTag(gateCamera.mEntityID, "GateCamera");
  }
}
using IGE.Utils;
using System.Numerics;

public class BridgeHorrorTrigger : Entity
{
  public PlayerMove playerMove;
  public float percentageToReduce = 0.001f;
  public string[] lookBackDialogue;
  public Entity theOtherBridgeHorrorTrigger;
  public Entity footstepSoundEntity;
  public Transition transition;

  private float oldPlayerWalkingSpeed;
  private float oldPlayerRunSpeed;
  private float oldSensitivity;

  private bool horrorSequenceTriggered = false;
  private Vector3 bridgeStart;
  public Vector3 bridgeEnd;
  private float bridgeLength;
  private TutorialDialogue tutorialDialogue;

  public Vector3 initialCameraForward;
  private bool lookBackTriggered = false;
  private bool forceLookForward = false;
  private float lastFootstepProgress = 0f;



    private bool whispersPlayed = false;
    private bool threatAudioPlayed = false;


    void Start()
  {
    tutorialDialogue = FindObjectOfType<TutorialDialogue>();
    oldPlayerWalkingSpeed = playerMove.walkingSpeed;
    oldPlayerRunSpeed = playerMove.runSpeed;
    oldSensitivity = playerMove.sensitivity;
    Cleanup();
  }

  void Update()
  {
    if (InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
    {
      TriggerHorrorSequence();
    }

    if (horrorSequenceTriggered)
    {
      ApplyHorrorEffects();
      CheckLookBack();
    }

    if (forceLookForward && !tutorialDialogue.isInDialogueMode)
    {
      transition.StartTransition(false, 1f, Transition.TransitionType.FADE);
      ForceLookForward();
      forceLookForward = false;
      lookBackTriggered = false;
    }
  }

  private void TriggerHorrorSequence()
  {
    SetActive(false);
    theOtherBridgeHorrorTrigger.SetActive(false);
    horrorSequenceTriggered = true;
    InternalCalls.SetShaderState(1, true);

    bridgeStart = InternalCalls.GetWorldPosition(playerMove.mEntityID);
    bridgeLength = Vector3.Distance(bridgeStart, bridgeEnd);
  }

  private void Cleanup()
  {
    InternalCalls.SetShaderState(1, false);
    playerMove.walkingSpeed = oldPlayerWalkingSpeed;
    playerMove.runSpeed = oldPlayerRunSpeed;
    playerMove.sensitivity = oldSensitivity;
    horrorSequenceTriggered = false;
    lookBackTriggered = false;
    forceLookForward = false;
    lastFootstepProgress = 0f;

        whispersPlayed = false;
        threatAudioPlayed = false;
        InternalCalls.StopSound(mEntityID, "phoneWhispers");
        InternalCalls.StopSound(mEntityID, "phoneVoiceWhispers");
    }

  private void ApplyHorrorEffects()
  {
    float progress = Mathf.Clamp01(Vector3.Distance(
        InternalCalls.GetWorldPosition(playerMove.mEntityID), bridgeStart) / bridgeLength);

    Debug.Log("Horror Effect Progress: " + progress);

    playerMove.walkingSpeed = Easing.Linear(oldPlayerWalkingSpeed * 0.4f, oldPlayerWalkingSpeed * percentageToReduce, progress);
    playerMove.runSpeed = Easing.Linear(oldPlayerRunSpeed * 0.4f, oldPlayerRunSpeed * percentageToReduce, progress);
    playerMove.sensitivity = Easing.Linear(oldSensitivity, oldSensitivity * percentageToReduce, progress);

    InternalCalls.SetVignetteStrength(progress);

    if (progress - lastFootstepProgress >= 0.15f)
    {
      lastFootstepProgress = progress;
      InternalCalls.PlaySound(footstepSoundEntity.mEntityID, "MonsterFoot");
    }

        if (progress > 0.3f && !whispersPlayed)
        {
            whispersPlayed = true;
            InternalCalls.PlaySound(mEntityID, "phoneWhispers"); // Assuming it's a looping ambient sound
        }

        // Stage 2: Whispering stops, deeper voice starts near full vignette
        if (progress > 0.6f && !threatAudioPlayed)
        {
            threatAudioPlayed = true;
            InternalCalls.StopSound(mEntityID, "phoneWhispers");
            InternalCalls.PlaySound(mEntityID, "phoneVoiceWhispers");
            }
            if (progress > 0.85f)
    {
      Cleanup();
    }
  }

  private void CheckLookBack()
  {
    if (lookBackTriggered || tutorialDialogue.isInDialogueMode) return;

    playerMove.useScriptRotation = true;
    InternalCalls.UpdatePhysicsToTransform(playerMove.mEntityID);

    Vector3 currentCameraForward = InternalCalls.GetCameraForward();
    float dot = Vector3.Dot(Vector3.Normalize(initialCameraForward), Vector3.Normalize(currentCameraForward));
    float angle = Mathf.Acos(dot) * (180f / Mathf.PI);

    if (angle > 100f)
    {
      lookBackTriggered = true;
      tutorialDialogue.SetDialogue(lookBackDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Disturbed });
      forceLookForward = true;
      playerMove.canLook = false;
    }
  }

  private void ForceLookForward()
  {
    Vector3 forward = initialCameraForward;
    forward.Y = 0;
    forward = Vector3.Normalize(forward);

    playerMove.yaw = Mathf.Atan2(forward.X, forward.Z) * (180f / Mathf.PI) + 180f;
    playerMove.pitch = 0;

    InternalCalls.UpdatePhysicsToTransform(playerMove.mEntityID);
  }
}
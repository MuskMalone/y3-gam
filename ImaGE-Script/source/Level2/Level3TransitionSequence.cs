using IGE.Utils;
using System.Numerics;

public class Level3TransitionSequence : Entity
{
  public Entity playerCam;
  public Fragment fragment;
  public Entity sinkHole, groundSmoke, aftermathSmoke;
  public Entity mainGround, sinkHoleVictims;
  public Entity invisibleBarrier, animGroundCollider;
  public Entity hexTable;
  public PlayerInteraction playerInteraction; // Handles raycasting
  public float delayAfterCollection, delayAfterGroundSmoke;

  // camera shake stuff
  public float startShakeIntensity = 1f, endShakeIntensity = 2.5f;
  public float shakeWindowDuration = 1f;

  private enum State
  {
    WAITING,
    GROUND_SHAKE,
    CAVE_IN,
    DROP
  }

  private State currState = State.WAITING;
  private string entityTag;
  private float timeElapsed = 0f, shakeTimeElapsed = 0f;
  private Vector3 originalCamPos;

  public Level3TransitionSequence() : base() { }

  void Start()
  {
    if (playerInteraction == null)
    {
      Debug.LogError("[TransitionToLevel3.cs] PlayerInteraction Script not found!");
    }

    entityTag = InternalCalls.GetTag(mEntityID);

    mainGround.SetActive(true);
    sinkHole.SetActive(false);
    groundSmoke.SetActive(false);
    aftermathSmoke.SetActive(false);
  }

  void Update()
  {
    switch (currState)
    {
      case State.WAITING:
        if (fragment.IsFragmentCollected())
        {
          timeElapsed += Time.deltaTime;

          if (timeElapsed >= delayAfterCollection)
          {
            timeElapsed = 0f;
            originalCamPos = playerCam.GetComponent<Transform>().position;
            groundSmoke.SetActive(true);
            invisibleBarrier.SetActive(true);
            currState = State.GROUND_SHAKE;
          }
        }

        break;

      case State.GROUND_SHAKE:
        {
          timeElapsed += Time.deltaTime;

          if (timeElapsed >= delayAfterGroundSmoke)
          {
            TransformHexRoom();
            currState = State.CAVE_IN;
            return;
          }

          float shakeIntensity = Mathf.Lerp(startShakeIntensity, endShakeIntensity, timeElapsed / delayAfterGroundSmoke);
          ShakeCamera(shakeIntensity);

          break;
        }

      case State.CAVE_IN:
        {
          InternalCalls.SetGravityFactor(playerInteraction.mEntityID, 100f);

          break;
        }
    }
  }

  private void TransformHexRoom()
  {
    mainGround.SetActive(false);
    sinkHoleVictims.SetActive(false);
    groundSmoke.SetActive(false);
    animGroundCollider.SetActive(false);
    hexTable.SetActive(false);

    sinkHole.SetActive(true);
    aftermathSmoke.SetActive(true);
  }

  private void ShakeCamera(float shakeIntensity)
  {
    
    if (shakeTimeElapsed < shakeWindowDuration)
    {
      float x = Mathf.RandRange(-shakeIntensity, shakeIntensity);
      float y = Mathf.RandRange(-shakeIntensity, shakeIntensity);
      float z = Mathf.RandRange(-shakeIntensity, shakeIntensity);

      playerCam.GetComponent<Transform>().position = originalCamPos + new Vector3(x, y, z);
    }
    else
    {
      playerCam.GetComponent<Transform>().position = originalCamPos;
      shakeTimeElapsed -= shakeWindowDuration;
    }
  }
}

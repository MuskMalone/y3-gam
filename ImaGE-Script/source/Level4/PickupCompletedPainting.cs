using IGE.Utils;

public class PickupCompletedPainting : Entity, IInventoryItem
{
  public Entity _Image;
  public Level4Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public Entity EToPickUpUI;
  public Entity fragmentBloom1;
  public Entity fragmentBloom2;
  public Entity fragmentBloom3;
  public Entity fragmentBloom4;
  public Entity momBehindGatePainting;
  public float maximumBloom = 3f;

  public Entity playerCamera;
  private bool isBeingPickedUp = false;
  public float finalDistanceAwayFromCamAfterPickup = 2f;
  public PlayerMove playerMove;

  private float currentBloomIntensity = 0f;
  private float bloomProgress = 0f;
  private float lerpDuration = 1.5f;
  private float elapsedTime = 0f;
  private float initialBloom;

  private enum State
  {
    NONE,
    BLOOM_ACTIVATE,
    CHANGE_INTO_PAINTING,
    COLLECT_PAINTING,
    END
  }
  private State currState = State.NONE;

  public string Name
  {
    get
    {
      return "MotherBehindGatePainting";
    }
  }

  public Entity Image
  {
    get
    {
      return _Image;
    }

    set
    {
      _Image = value;
    }
  }

  public void OnPickup()
  {
    SetActive(false);
  }

  public void OnUsed()
  {

  }

  public PickupCompletedPainting() : base()
  {

  }

  void Start()
  {
    EToPickUpUI?.SetActive(false);
    momBehindGatePainting?.SetActive(false);
    initialBloom = InternalCalls.GetBloomIntensity(fragmentBloom1.mEntityID);
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
            currState = State.BLOOM_ACTIVATE;
            isPaintingHit = false;
            EToPickUpUI.SetActive(isPaintingHit);
            return;
          }
          EToPickUpUI.SetActive(isPaintingHit);
          break;
        }

      case State.BLOOM_ACTIVATE:
        {
          elapsedTime += Time.deltaTime;
          bloomProgress = Mathf.Clamp01(elapsedTime / lerpDuration);
          currentBloomIntensity = Easing.Linear(initialBloom, maximumBloom, bloomProgress);
          InternalCalls.SetBloomIntensity(fragmentBloom1.mEntityID, currentBloomIntensity);
          InternalCalls.SetBloomIntensity(fragmentBloom2.mEntityID, currentBloomIntensity);
          InternalCalls.SetBloomIntensity(fragmentBloom3.mEntityID, currentBloomIntensity);
          InternalCalls.SetBloomIntensity(fragmentBloom4.mEntityID, currentBloomIntensity);

          if (bloomProgress >= 1.0f)
          {
            elapsedTime = 0;
            bloomProgress = 0;
            currState = State.CHANGE_INTO_PAINTING;
            return;
          }

          break;
        }

      case State.CHANGE_INTO_PAINTING:
        {
          elapsedTime += Time.deltaTime;
          bloomProgress = Mathf.Clamp01(elapsedTime / lerpDuration);
          currentBloomIntensity = Easing.Linear(maximumBloom, 0, bloomProgress);
          InternalCalls.SetBloomIntensity(fragmentBloom1.mEntityID, currentBloomIntensity);
          InternalCalls.SetBloomIntensity(fragmentBloom2.mEntityID, currentBloomIntensity);
          InternalCalls.SetBloomIntensity(fragmentBloom3.mEntityID, currentBloomIntensity);
          InternalCalls.SetBloomIntensity(fragmentBloom4.mEntityID, currentBloomIntensity);

          if (bloomProgress >= 1.0f)
          {
            InternalCalls.SetBloomIntensity(fragmentBloom1.mEntityID, 0);
            InternalCalls.SetBloomIntensity(fragmentBloom2.mEntityID, 0);
            InternalCalls.SetBloomIntensity(fragmentBloom3.mEntityID, 0);
            InternalCalls.SetBloomIntensity(fragmentBloom4.mEntityID, 0);
            momBehindGatePainting.SetActive(true);
            currState = State.COLLECT_PAINTING;
            return;
          }

          break;
        }

      case State.COLLECT_PAINTING:
        {
          if (isBeingPickedUp)
          {
            if (Pickup.MoveAndShrink(this, playerInteraction.mEntityID, playerCamera.mEntityID, finalDistanceAwayFromCamAfterPickup))
            {
              InternalCalls.PlaySound(mEntityID, "PickupObjects");
              isBeingPickedUp = false;
              playerMove.UnfreezePlayer();
              Debug.Log("Trying to add to inventory");
              inventoryScript.Additem(this);
              currState = State.END;
            }
            return;
          }

          bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
          if (Input.GetKeyTriggered(KeyCode.E) && isPaintingHit)
          {
            isPaintingHit = false;
            EToPickUpUI.SetActive(isPaintingHit);
            isBeingPickedUp = true;
            playerMove.FreezePlayer();
            return;
          }
          EToPickUpUI.SetActive(isPaintingHit);
          break;
        }

      case State.END:
        {

          break;
        }
    }
  }
}
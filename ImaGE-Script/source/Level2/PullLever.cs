using IGE.Utils;
using System;

public class PullLever : Entity
{
  public PlayerInteraction playerInteraction;
  public HexTableOrb orb;
  public Entity leverLight;
  public float timeBeforeCamSwitch = 0.1f;

  public Entity platformLight;
  private LeverManager leverManager; // Reference to the manager
  private string thisLeverTag;
  private bool leverPulled = false;
  private float timeElapsed = 0f;
  private Level2Inventory level2InventoryScript;
  private PictureAlign picAlignScript;

  public PullLever() : base() { }

  void Start()
  {
    level2InventoryScript = FindObjectOfType<Level2Inventory>();
    picAlignScript = FindObjectOfType<PictureAlign>();
    leverManager = FindObjectOfType<LeverManager>();

    thisLeverTag = InternalCalls.GetTag(mEntityID);
    InternalCalls.SetLightIntensity(leverLight.mEntityID, 0.0f);
    InternalCalls.SetLightIntensity(platformLight.mEntityID, 51.0f);
    }
  void Update()
  {
    if (!leverPulled)
    {
      if (Input.GetMouseButtonTriggered(0))
      {
        string hitObjectTag = playerInteraction.RayHitString; // Object the player clicked on

        if (hitObjectTag == thisLeverTag) // Check if player clicked this lever
        {
          ActivateLever();
        }
      }

      return;
    }

    // when animation has finished, notify lever manager
    if (!GetComponent<Animation>().IsPlaying())
    {
      timeElapsed += Time.deltaTime;

      if (timeElapsed >= timeBeforeCamSwitch)
      {
        orb.Rise();
        leverManager?.LeverPulled();
        InternalCalls.SetLightIntensity(leverLight.mEntityID, 5.0f);
                InternalCalls.SetLightIntensity(platformLight.mEntityID, 0.0f);
                Destroy(this);  // we no longer need this script
      }
    }
  }

  public bool LeverPulled() { return leverPulled; }

  private void ActivateLever()
  {
    InternalCalls.PlaySound(mEntityID, "LeverUp");
    GetComponent<Animation>().Play("SwitchOff");
    leverPulled = true;
    level2InventoryScript.CloseInventoryAndUnselectAllItems();
    picAlignScript.ClearAll();
  }
}

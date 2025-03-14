using IGE.Utils;
using System;

public class PullLever : Entity
{
  public PlayerInteraction playerInteraction;
  public HexTableOrb orb;
  public Entity leverLight;
  public float timeBeforeCamSwitch = 0.1f;

  private LeverManager leverManager; // Reference to the manager
  private string thisLeverTag;

  private bool leverPulled = false;
  private float timeElapsed = 0f;
  public PullLever() : base() { }

  void Start()
  {
    thisLeverTag = InternalCalls.GetTag(mEntityID);
    leverManager = FindObjectOfType<LeverManager>();
    leverManager?.AddOrb(ref orb);  // pass ref to leverManager
    InternalCalls.SetLightIntensity(leverLight.mEntityID, 0.0f);
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
    if (!InternalCalls.IsPlayingAnimation(mEntityID))
    {
      timeElapsed += Time.deltaTime;

      if (timeElapsed >= timeBeforeCamSwitch)
      {
        orb.Rise();
        leverManager?.LeverPulled();
        InternalCalls.SetLightIntensity(leverLight.mEntityID, 5.0f);
        Destroy(this);  // we no longer need this script
      }
    }
  }

  private void ActivateLever()
  {
    InternalCalls.PlaySound(mEntityID, "IncoherentWhispers");
    InternalCalls.PlayAnimation(mEntityID, "SwitchOff");
    leverPulled = true;
  }
}

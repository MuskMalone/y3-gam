using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class HammerGlow : Entity
{
  public Entity player;
  public Entity hammerPart1, hammerPart2;
  public HammerLevel3 hammerItem;
  public float glowInterval;
  public float maxBloom;

  private float timeElapsed = 0f;
  private float glowHoldDuration;
  private bool glowing = false, flipped = false;

  // Start is called before the first frame update
  void Start()
  {
    glowHoldDuration = glowInterval * 0.5f;
  }

  // Update is called once per frame
  void Update()
  {
    // once the hammer is picked up, stop this script
    if (hammerItem.GetState() == HammerLevel3.HammerState.PICKING_UP)
    {
      Reset();
      Destroy();
      return;
    }

    if (InternalCalls.OnTriggerEnter(mEntityID, player.mEntityID))
    {
      glowing = true;
    }
    // if player leaves, reset
    else if (InternalCalls.OnTriggerExit(mEntityID, player.mEntityID))
    {
      Reset();
    }

    if (glowing)
    {
      timeElapsed += Time.deltaTime;

      float t = Mathf.SmoothStep(Mathf.Clamp(timeElapsed, 0f, maxBloom) / glowInterval);

      SetBloom(flipped ? 1 - t : t);

      if (timeElapsed >= glowInterval)
      {
        flipped = !flipped;
        timeElapsed -= glowInterval;
      }
    }
  }

  void SetBloom(float intensity)
  {
    InternalCalls.SetBloomIntensity(hammerPart1.mEntityID, intensity);
    InternalCalls.SetBloomIntensity(hammerPart2.mEntityID, intensity);
  }

  void Reset()
  {
    timeElapsed = 0f;
    flipped = glowing = false;
    SetBloom(0f);
  }
}

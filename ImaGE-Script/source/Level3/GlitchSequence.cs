using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class GlitchSequence : Entity
{
  public float glitchSequenceTotalDuration = 5f;
  public Entity[] lightsToFlicker;
  public int glitchShaderIdx = 0;
  public Entity triggerEntity;
  public PlayerMove playerMove;

  private float glitchSequenceTimer = 0f;
  private bool triggeredOnce = false;

  public GlitchSequence() : base()
  {

  }

  void Start()
  {
    
  }

  void Update()
  {
    if (!triggeredOnce && InternalCalls.OnTriggerEnter(triggerEntity.mEntityID, playerMove.mEntityID))
    {
      triggeredOnce = true;
      InternalCalls.SetShaderState((uint)glitchShaderIdx, true);
      InternalCalls.PlaySound(triggerEntity.mEntityID, "GlitchSFX");
    }

    if (triggeredOnce)
    {
      glitchSequenceTimer += Time.deltaTime;

      if (glitchSequenceTimer >= glitchSequenceTotalDuration)
      {
        InternalCalls.SetShaderState((uint)glitchShaderIdx, false);
        SetActive(false);
      }
    }
  }
}
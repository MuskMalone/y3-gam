using IGE.Utils;
using System;

public class LightFlicker : Entity
{
  public Entity lightSFX;
  public Entity lightSource;
  public Entity bloom;
  public float minFlickerTime = 0.5f;
  public float maxFlickerTime = 2f;
  public bool activateFlicker = false;
  public Entity corridorTrigger;
  public PlayerMove playerMove;
  private float flickerTimer;

  private float flickerSpeed = 2f; // Adjust for flicker smoothness
  private float noiseOffset; // Unique noise pattern per light
  private Random random = new Random();
  private float prevNoiseValue = 0.0f;
  public LightFlicker() : base()
  {
  }
  private void PlayRandomPing()
    {
        if (lightSFX.mEntityID != uint.MaxValue)
        {
            int soundNumber = random.Next(1, 3);
            string soundName = $"Ping{soundNumber}";
            //InternalCalls.SetSoundVolume(mEntityID, soundName, footstepVolume);
            InternalCalls.PlaySound(lightSFX.mEntityID, soundName);
        }
    }
  void Start()
  {
    noiseOffset = Mathf.RandRange(0f, 100f); // Ensures unique flicker pattern per light
    ResetFlickerTimer(); // Start with a random flicker delay
  }

  void Update()
  {
    if (!activateFlicker)
    {
      if (InternalCalls.OnTriggerEnter(corridorTrigger.mEntityID, playerMove.mEntityID))
      {
        activateFlicker = true;
      }
    }

    if (activateFlicker)
    {
      flickerTimer -= Time.deltaTime;
      if (flickerTimer <= 0)
      {
        ResetFlickerTimer();
      }

      // Perlin Noise-based flickering effect
      float noiseValue = Easing.PerlinNoise(Time.gameTime * flickerSpeed + noiseOffset);
      float interpolatedIntensity = Mathf.Lerp(0.2f, 4.0f, noiseValue);
            if (lightSFX.mEntityID != uint.MaxValue)
            {
                //InternalCalls.SetSoundVolume(lightSFX.mEntityID, "ElectricBuzz", (interpolatedIntensity - 0.2f) / (10.0f - 0.2f));
            }
            if (prevNoiseValue - noiseValue > float.Epsilon)
            {
                //PlayRandomPing();
            }
            prevNoiseValue = noiseValue;
            InternalCalls.SetLightIntensity(lightSource.mEntityID, interpolatedIntensity);
      InternalCalls.SetBloomIntensity(bloom.mEntityID, interpolatedIntensity);
    }
  }

  private void ResetFlickerTimer()
  {
    flickerTimer = Mathf.RandRange(minFlickerTime, maxFlickerTime); // Random delay before flicker starts
  }
}
using IGE.Utils;
using System;

public class CandleFlicker : Entity
{
    public Entity candleLight;  // Main Point Light Entity
    public Entity candleBloom;  // Bloom Effect Entity
    public Entity secondaryLight;  // Secondary fill light for softer flickering

    // Flickering Timing
    public float minFlickerTime = 0.3f; // Faster flicker than electric light
    public float maxFlickerTime = 1.2f;

    // Flickering Speed
    public float flickerSpeed = 1.5f; // Softer flickering for candle

    // Main Light Intensity Range
    public float minLightIntensity = 0.5f;
    public float maxLightIntensity = 2.5f;

    // Bloom Intensity Range (independent from light)
    public float minBloomIntensity = 0.3f;
    public float maxBloomIntensity = 1.8f;

    // Secondary Light Intensity (weaker, more subtle flickering)
    public float minFillIntensity = 0.2f;
    public float maxFillIntensity = 0.6f;

    private float flickerTimer;
    private float noiseOffset;
    private Random random = new Random();

    public CandleFlicker() : base() { }

    void Start()
    {
        noiseOffset = Mathf.RandRange(0f, 100f); // Unique flicker pattern for each candle
        ResetFlickerTimer(); // Start with a random flicker delay
    }

    void Update()
    {
        flickerTimer -= Time.deltaTime;
        if (flickerTimer <= 0)
        {
            ResetFlickerTimer();
        }

        // Perlin Noise-based flickering effect (smooth, organic flickers)
        float noiseValue = Easing.PerlinNoise(Time.gameTime * flickerSpeed + noiseOffset);

        // Separate Light and Bloom intensity calculations
        float flickerLightIntensity = Mathf.Lerp(minLightIntensity, maxLightIntensity, noiseValue);
        float flickerBloomIntensity = Mathf.Lerp(minBloomIntensity, maxBloomIntensity, noiseValue);

        // Secondary flickering effect (slightly delayed and weaker)
        float secondaryNoiseValue = Easing.PerlinNoise((Time.gameTime + 0.3f) * flickerSpeed + noiseOffset);
        float flickerSecondaryIntensity = Mathf.Lerp(minFillIntensity, maxFillIntensity, secondaryNoiseValue);

        // Apply the flicker effect
        InternalCalls.SetLightIntensity(candleLight.mEntityID, flickerLightIntensity);
        InternalCalls.SetBloomIntensity(candleBloom.mEntityID, flickerBloomIntensity);

        // Apply secondary fill light flickering
        InternalCalls.SetLightIntensity(secondaryLight.mEntityID, flickerSecondaryIntensity);
    }

    private void ResetFlickerTimer()
    {
        flickerTimer = Mathf.RandRange(minFlickerTime, maxFlickerTime); // Random flicker timing
    }
}

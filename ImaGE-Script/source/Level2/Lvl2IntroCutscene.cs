using IGE.Utils;
using System;
using System.Numerics;

public class Lvl2IntroCutscene : Entity
{
    public PlayerMove playerMove;
    public Entity firstSilhouette;
    public Entity secondSilhouette;
    public Entity thirdSilhouette;
    public Entity fourthSilhouette;
    public Entity fifthSilhouette;
    public Entity sixthSilhouette;
    private Entity[] BeginningSilhouetteSequence;
    public Entity fadeImage;
    public Entity mainCamera;

    private bool triggerFadeTransition = false; // ❌ Doesn't start automatically
    private bool isInFadeTransition = false;
    private float fadeTransitionTimer = 0f;
    private float fadeStartTime;
    private bool isFadingOut = true;
    private float fadeProgress = 0f;
    public float fadeDuration = 3f;
    private float fadeTime = 0f;

    private bool isInSillouetteSequence = false;
    private int currentSillouetteIndex = -1;
    private float sillouetteElapsedTime = 0f;
    private const float activationInterval = 1.0f;

    private bool sequenceStarted = false; // ✅ Ensures it runs only once

    public Lvl2IntroCutscene() : base() { }

    void Start()
    {
        BeginningSilhouetteSequence = new Entity[6];
        BeginningSilhouetteSequence[0] = firstSilhouette;
        BeginningSilhouetteSequence[1] = secondSilhouette;
        BeginningSilhouetteSequence[2] = thirdSilhouette;
        BeginningSilhouetteSequence[3] = fourthSilhouette;
        BeginningSilhouetteSequence[4] = fifthSilhouette;
        BeginningSilhouetteSequence[5] = sixthSilhouette;

        for (int i = 0; i < BeginningSilhouetteSequence.Length; i++)
        {
            BeginningSilhouetteSequence[i]?.SetActive(false);
        }

        if (playerMove == null)
        {
            Console.WriteLine("[Lvl2IntroCutscene] ERROR: PlayerMove Script Entity not found!");
            return;
        }
    }

    void Update()
    {
        // ✅ Check if player enters THIS trigger zone (since script is on the trigger)
        if (!sequenceStarted && InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
        {
            sequenceStarted = true;
            triggerFadeTransition = true; // ✅ Start the cutscene
            Console.WriteLine("[Lvl2IntroCutscene] Player entered trigger! Starting cutscene...");
        }

        if (triggerFadeTransition)
        {
            StartFade();
        }

        if (isInFadeTransition)
        {
            float elapsed = Time.gameTime - fadeStartTime;
            fadeTransitionTimer = elapsed;

            float alpha = Mathf.Lerp(1f, 0f, fadeTransitionTimer / fadeDuration);
            InternalCalls.SetImageColor(fadeImage.mEntityID, new Vector4(1, 1, 1, alpha));

            if (fadeTransitionTimer >= fadeDuration)
            {
                EndFade();
            }
        }

        if (isInSillouetteSequence)
        {
            sillouetteElapsedTime += Time.deltaTime;

            if (currentSillouetteIndex == -1)
            {
                currentSillouetteIndex = 0;
                Entity firstSprite = BeginningSilhouetteSequence[currentSillouetteIndex];
                firstSprite.SetActive(true);

                fadeProgress = 0f;
                Vector4 initialColor = InternalCalls.GetSprite2DColor(firstSprite.mEntityID);
                InternalCalls.SetSprite2DColor(firstSprite.mEntityID, new Vector4(initialColor.X, initialColor.Y, initialColor.Z, 0f));

                isFadingOut = true;

                Debug.Log($"[Lvl2IntroCutscene] Initialized First Sprite: {firstSprite.mEntityID}");
            }

            if (currentSillouetteIndex >= 0 && currentSillouetteIndex < BeginningSilhouetteSequence.Length)
            {
                Entity currentSprite = BeginningSilhouetteSequence[currentSillouetteIndex];

                if (isFadingOut)
                {
                    fadeProgress += Time.deltaTime / fadeDuration;
                    Vector4 initialColor = InternalCalls.GetSprite2DColor(currentSprite.mEntityID);
                    float newAlpha = 1f - Mathf.Clamp01(fadeProgress);
                    InternalCalls.SetSprite2DColor(currentSprite.mEntityID, new Vector4(initialColor.X, initialColor.Y, initialColor.Z, newAlpha));

                    Debug.Log($"[Lvl2IntroCutscene] Fading Out - Sprite: {currentSprite.mEntityID}, Alpha: {newAlpha}");

                    if (fadeProgress >= 1f)
                    {
                        currentSprite.SetActive(false);
                        fadeProgress = 0f;
                        isFadingOut = false;
                    }
                }
            }

            if (!isFadingOut && sillouetteElapsedTime >= activationInterval)
            {
                sillouetteElapsedTime = 0f;
                currentSillouetteIndex++;

                if (currentSillouetteIndex < BeginningSilhouetteSequence.Length)
                {
                    Entity nextSprite = BeginningSilhouetteSequence[currentSillouetteIndex];
                    nextSprite.SetActive(true);

                    fadeProgress = 0f;
                    Vector4 initialColor = InternalCalls.GetSprite2DColor(nextSprite.mEntityID);
                    InternalCalls.SetSprite2DColor(nextSprite.mEntityID, new Vector4(initialColor.X, initialColor.Y, initialColor.Z, 0f));

                    isFadingOut = true;

                    Debug.Log($"[Lvl2IntroCutscene] Activated Sprite: {nextSprite.mEntityID}");
                }
                else
                {
                    EndSiloutetteSequence();
                }
            }
        }
    }

    public void StartSilhouetteSequence()
    {
        isInSillouetteSequence = true;
        //playerMove.FreezePlayer();
    }

    private void EndSiloutetteSequence()
    {
        //playerMove.UnfreezePlayer();
        isInSillouetteSequence = false;
    }

    private void StartFade()
    {
        triggerFadeTransition = false;
        isInFadeTransition = true;
        fadeImage.SetActive(true);
        //playerMove.FreezePlayer();

        fadeStartTime = Time.gameTime;
    }

    private void EndFade()
    {
        //playerMove.UnfreezePlayer();
        isInFadeTransition = false;
        fadeImage.SetActive(false);
        StartSilhouetteSequence(); // ✅ Start silhouettes after fade
    }
}

using IGE.Utils;
using System;
using System.Numerics;
using System.Runtime;

public class Lvl2IntroCutscene : Entity
{
    public PlayerMove playerMove;
    //public Entity firstSilhouette;
    public Entity secondSilhouette;
    public Entity thirdSilhouette;
    public Entity fourthSilhouette;
    public Entity fifthSilhouette;
    public Entity sixthSilhouette;
    private Entity[] BeginningSilhouetteSequence;
    public Entity fadeImage;
    public Entity mainCamera;

    private bool triggerFadeTransition = false;
    private bool isInFadeTransition = false;
    private float fadeTransitionTimer = 0f;
    private float fadeStartTime;
    private bool isFadingOut = true;
    private float fadeProgress = 0f;
    public float fadeDuration = 3f;

    private bool isInSillouetteSequence = false;
    private int currentSillouetteIndex = -1;
    private float sillouetteElapsedTime = 0f;
    private const float activationInterval = 1.0f;

    private Quaternion targetRot = Mathf.EulertoQuat(new Vector3(90, 0, 130));

    //private bool rotateAfterFirstSilhouette = false;
    //private bool isRotatingPlayer = false;
    //private float targetRotationY = 0; // Target rotation in degrees
    //private float rotationSpeed = 50.0f; // Degrees per second

    private bool sequenceStarted = false;

    public Lvl2IntroCutscene() : base() { }

    void Start()
    {
        //BeginningSilhouetteSequence = new Entity[6]
        //{
        //    firstSilhouette, secondSilhouette, thirdSilhouette,
        //    fourthSilhouette, fifthSilhouette, sixthSilhouette
        //};

        BeginningSilhouetteSequence = new Entity[5]
        {
            secondSilhouette, thirdSilhouette,
            fourthSilhouette, fifthSilhouette, sixthSilhouette
        };

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
        //if (isRotatingPlayer)
        //{
        //    RotatePlayerToTarget();
        //    return;
        //}

        if (!sequenceStarted && InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
        {
            sequenceStarted = true;
            triggerFadeTransition = true;
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
                        //if (currentSillouetteIndex == 0)
                        //{
                        //    rotateAfterFirstSilhouette = true;
                        //}
                    }
                }
            }

            if (!isFadingOut && sillouetteElapsedTime >= activationInterval)
            {
                //if (rotateAfterFirstSilhouette)
                //{
                //    rotateAfterFirstSilhouette = false;
                //    StartPlayerRotation();
                //    return;
                //}
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

    //private void RotatePlayerToTarget()
    //{
    //    if (playerMove == null) return;

    //    // ✅ Get the player's current rotation (Euler)
    //    Vector3 currentRotation = InternalCalls.GetRotationEuler(playerMove.mEntityID);
    //    float deltaTime = Time.deltaTime;

    //    // ✅ Use Lerp for smooth rotation
    //    float newRotationY = Mathf.Lerp(currentRotation.Y, targetRotationY, deltaTime * rotationSpeed);

    //    // ✅ Create a modifiable variable
    //    Vector3 newRotation = new Vector3(currentRotation.X, newRotationY, currentRotation.Z);

    //    // ✅ Pass a variable instead of a temporary value
    //    InternalCalls.SetRotationEuler(playerMove.mEntityID, ref newRotation);

    //    // ✅ Debugging to check values
    //    Console.WriteLine($"Current Rotation: {currentRotation.Y}, Target: {targetRotationY}, New: {newRotation.Y}");

    //    // ✅ Stop rotating when close enough
    //    if (Math.Abs(newRotationY - targetRotationY) < 0.1f)
    //    {
    //        Vector3 finalRotation = new Vector3(currentRotation.X, targetRotationY, currentRotation.Z);
    //        InternalCalls.SetRotationEuler(playerMove.mEntityID, ref finalRotation);
    //        isRotatingPlayer = false;
    //        Console.WriteLine("Rotation Complete!");
    //        sillouetteElapsedTime = activationInterval;
    //    }
    //}



    //private void StartPlayerRotation()
    //{
    //    if (playerMove == null) return;

    //    targetRotationY = 0; // ✅ Target facing direction
    //    isRotatingPlayer = true;
    //}

    private void StartFade()
    {
        triggerFadeTransition = false;
        isInFadeTransition = true;
        fadeImage.SetActive(true);
        
        SnapPlayerToTrigger();
        playerMove.FreezePlayer();
        fadeStartTime = Time.gameTime;
    }

    private void EndFade()
    {
        //playerMove.UnfreezePlayer();
        isInFadeTransition = false;
        fadeImage.SetActive(false);
        StartSilhouetteSequence();
    }

    private void SnapPlayerToTrigger()
    {
        if (playerMove == null) return;
        playerMove.canLook = false;
        Vector3 triggerPosition = InternalCalls.GetPosition(mEntityID);

        InternalCalls.SetPosition(playerMove.mEntityID, ref triggerPosition);
        Console.WriteLine("OgRot" + InternalCalls.GetMainCameraRotation(mainCamera.mEntityID));
       //InternalCalls.SetRotation(mainCamera.mEntityID, ref targetRot);
        playerMove.SetRotation(new Vector3(73, 65, 0));
        Console.WriteLine("NewRot" + InternalCalls.GetMainCameraRotation(mainCamera.mEntityID));
        //playerMove.canLook = true;
    }

    private void StartSilhouetteSequence()
    {
        isInSillouetteSequence = true;
        //playerMove.FreezePlayer();
    }

    private void EndSiloutetteSequence()
    {
        //playerMove.UnfreezePlayer();
        isInSillouetteSequence = false;
    }
}
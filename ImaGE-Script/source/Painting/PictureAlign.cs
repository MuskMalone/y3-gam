using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
using System.Text.RegularExpressions;
using System.Security;

public class PictureAlign : Entity
{
  public Entity player;               // Assign the player object
  public Entity mainCamera;           // Assign the main camera for rotation checking
  public Entity border;
  public Entity RightArrow;
  public Entity LeftArrow;
  public Entity UpArrow;
  public Entity DownArrow;
  //public Entity LeftClickText;
  private bool isBigPic;
  private bool toStop = true;
  private bool isTransitioning = false;
  public bool isNight = false;

  public float positionThreshold = 0.5f;  // Threshold for position alignment
  public float rotationThreshold = 2f;    // Threshold for rotation alignment (in degrees)

  private PlayerMove playerMove;          // Reference to the PlayerLook script
  public bool isFrozen = false;           // Check if the player is frozen
  public bool alignCheck = false;

  private Vector3 savedPosition;
  private Vector3 savedCameraEuler;
  private Quaternion savedCameraRotation;

  //For setting the borderSize
  private Vector3 bigBorderScale = new Vector3(2.340f, 0.790f, 0.771f);
  private Vector3 bigBorderPos = new Vector3(-0.004f, 0.030f, -2.810f);
  private Vector3 smallBorderScale = new Vector3(1.150f, 0.360f, 0.411f);
  private Vector3 smallBorderPos = new Vector3(1.620f, 0.001f, -2.810f);

  private HoldupUI currentImg;
  private string picture;

  delegate void PictureInteration();
    public bool startFade = false;
    //private TutorialFade tutorialFade;


    //For fading out after alignment
    public float fadeSpeed = 1.0f;
    private float currentAlpha = 1f;
    private bool hasFaded = false;
    private float timer = 0f;

    //For transition scene in the corridor
    public float moveDuration = 2.0f; // Total duration to move the camera
    private float elapsedTime = 0f;
    private bool isMovingCamera = false;
    private float startY;
    public float targetY = 5.0f; // Adjust as needed
    private CorridorTransitionFade corridorTransitionFadeScript;

    private ControlPanel2 controlPanelScript;

    // for inventory checks
    public bool isFading = false;

    //For pit puzzle
    private PitPuzzle pitPuzzleScript;

    void Start()
    {
        //tutorialFade = FindObjectOfType<TutorialFade>();
        // Initialize the movement and camera control components
        playerMove = player.FindObjectOfType<PlayerMove>();

        corridorTransitionFadeScript = FindObjectOfType<CorridorTransitionFade>();
        controlPanelScript = FindObjectOfType<ControlPanel2>();

        if (playerMove == null) Debug.LogError("PlayerMove component not found!");

        pitPuzzleScript = FindObjectOfType<PitPuzzle>();

    }

  void Update()
  {
    //if (Input.GetKeyTriggered(KeyCode.G))
    //  playerMove.canLook = false;
    //if (Input.GetKeyTriggered(KeyCode.M))
    //{
    //  toStop = true;
    //  alignCheck = true;
    //  //Debug.Log("Player is aligned.");
    //  playerMove.FreezePlayer();
    //  //currentImg.SetActive(false);
    //  isTransitioning = true;
    //  //if (LeftClickText != null)
    //  //  LeftClickText.SetActive(false);
    //  RightArrow.SetActive(false);
    //  LeftArrow.SetActive(false);
    //  UpArrow.SetActive(false);
    //  DownArrow.SetActive(false);
    //  //SetActive(false);
    //  InternalCalls.PlaySound(player.mEntityID, "PaintingMatchObject");
    //  isNight = true;
    //  playerMove.canLook = false;
    //}

    if (!toStop)
    {
      // Perform alignment checks and freeze the player if aligned
      if (IsActive() && isBigPic && IsAligned())
      {
        //if (LeftClickText != null)
        //  LeftClickText.SetActive(true);
        if (Input.GetMouseButtonDown(0))
        {
          toStop = true;
          alignCheck = true;
          //Debug.Log("Player is aligned.");
          playerMove.FreezePlayer();
          //currentImg.SetActive(false);
          isTransitioning = true;



          //if (LeftClickText != null)
          //  LeftClickText.SetActive(false);
          RightArrow.SetActive(false);
          LeftArrow.SetActive(false);
          UpArrow.SetActive(false);
          DownArrow.SetActive(false);
          //SetActive(false);
          InternalCalls.PlaySound(player.mEntityID, "PaintingMatchObject");
          isNight = true;
        }
        else
        {
          alignCheck = false;
        }
      }
      else
      {
        //if (LeftClickText != null)
        //  LeftClickText.SetActive(false);
        alignCheck = false;
      }
    }

    else
    {
      if (isTransitioning)
      {
        if (picture == "NightPainting")
        {
          FadeOut();
          if (ChangeSkyBox())
          {
            Console.WriteLine("NIght");
            InternalCalls.ChangeToolsPainting();
            if (hasFaded)
            {
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
          }
        }
        else if (picture == "ToolsPainting")
        {
          Console.WriteLine("Tool");
            FadeOut();
            InternalCalls.SpawnToolBox();
            if (hasFaded)
            {
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
        }
        else if (picture == "TutorialPainting")
        {
          Console.WriteLine("Tut");
          FadeOut();
          InternalCalls.SpawnOpenDoor();
            if (hasFaded)
            {
                currentImg.SetActive(false);
                SetActive(false);
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.TutorialRemoveItself();
                currentImg = null;
                //tutorialFade.StartFade();
                startFade = true;
                hasFaded = false;
            }
        }
        else if (picture == "CorridorPainting")
        {
            FadeOut();
            InternalCalls.SpawnTaraSilhouette();
            if (hasFaded)
            {
                TransitionCamera();
                //isTransitioning = false;
                //playerMove.UnfreezePlayer();
                //currentImg.RemoveItself();
                //currentImg = null;
                //InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\Level2.scn");
            }
        }

        else if (picture == "DionysusPainting")
        {
            FadeOut();
            controlPanelScript.SwitchMode(ControlPanel2.StatueType.DIONYSUS);
            if (hasFaded)
            {
                currentImg.SetActive(false);
                SetActive(false);
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.Level2RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
        }
        else if (picture == "ArtemisPainting")
        {
            FadeOut();
            controlPanelScript.SwitchMode(ControlPanel2.StatueType.ARTEMIS);
            if (hasFaded)
            {
                currentImg.SetActive(false);
                SetActive(false);
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.Level2RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
        }
        else if (picture == "ZeusPainting")
        {
            FadeOut();
            controlPanelScript.SwitchMode(ControlPanel2.StatueType.ZEUS);
            if (hasFaded)
            {
                currentImg.SetActive(false);
                SetActive(false);
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.Level2RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
        }
        else if (picture == "PoseidonPainting")
        {
            FadeOut();
            controlPanelScript.SwitchMode(ControlPanel2.StatueType.POSEIDON);
            if (hasFaded)
            {
                currentImg.SetActive(false);
                SetActive(false);
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.Level2RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
        }
        else if (picture == "PitPainting")
        {
            FadeOut();
            //controlPanelScript.SwitchMode(ControlPanel2.StatueType.POSEIDON);
            pitPuzzleScript.switchPlanks();
            if (hasFaded)
            {
                currentImg.SetActive(false);
                SetActive(false);
                playerMove.UnfreezePlayer();
                isTransitioning = false;
                currentImg.Level3RemoveItself();
                currentImg = null;
                hasFaded = false;
            }
        }
        else
        {
          Console.WriteLine("WHAt");
        }

      }
    }
  }


    public bool IsAligned()
  {
    float positionDistance = Vector3.Distance(player.GetComponent<Transform>().worldPosition, savedPosition);
    Vector2 currRot =playerMove.GetRotation();
    //Vector3 currWRot = mainCamera.GetComponent<Transform>().rotationWorldEuler;
    //Vector3 currLRot = mainCamera.GetComponent<Transform>().rotationEuler;
    Console.WriteLine("Curr:" + player.GetComponent<Transform>().worldPosition + " vs:" + savedPosition);
    Console.WriteLine("DIFF: " + positionDistance);
    bool aligned = true;

    if (positionDistance > positionThreshold)
    {
      DownArrow.SetActive(false);
      UpArrow.SetActive(false);
      RightArrow.SetActive(false);
      LeftArrow.SetActive(false);
      return false;// All checks passed, the player is aligned
    }
      //Console.WriteLine(Mathf.QuaternionAngle(currRot, savedCameraRotation));

    float xDiff = Mathf.AngleDifference(currRot.X,savedCameraEuler.X);
    float yDiff = Mathf.AngleDifference(currRot.Y,savedCameraEuler.Y);
    //Vector2 angleDiff = CalculatePitchAndYawDifferences(currWRot, savedCameraRotation);
    if (Math.Abs(xDiff) > rotationThreshold) // Ignore small differences (optional threshold)
    {

      aligned = false;
      if (xDiff > 0)
      {
        //Console.WriteLine("Look up");
        DownArrow.SetActive(false);
        UpArrow.SetActive(true);
      }
      else
      {
        //Console.WriteLine("Look Down");
        DownArrow.SetActive(true);
        UpArrow.SetActive(false);

      }

    }
    else
    {
      DownArrow.SetActive(false);
      UpArrow.SetActive(false);
    }

    //Determine yaw(left/ right) adjustment
    if (Math.Abs(yDiff) > rotationThreshold) // Ignore small differences (optional threshold)
    {
      //Console.WriteLine(yDiff + "::" + savedCameraEuler.Y + "vs" + currRot.Y);
      aligned = false;
      if (yDiff > 0)
      {
        //Console.WriteLine("Look left");
        RightArrow.SetActive(false);
        LeftArrow.SetActive(true);
      }
      else
      {
        //Console.WriteLine("Look right");
        RightArrow.SetActive(true);
        LeftArrow.SetActive(false);
      }
    }
    else
    {
      RightArrow.SetActive(false);
      LeftArrow.SetActive(false);
    }

    //if (aligned)
    //  Console.WriteLine("Player is aligned.");
    return aligned; // All checks passed, the player is aligned
  }

  // Method to retrieve the saved camera rotation
  public Quaternion GetSavedCameraRotation()
  {
    return savedCameraRotation;
  }


  public void SetTarget(Vector3 position, Quaternion rot, Vector3 euler, string s, HoldupUI UI)
  {
     savedPosition = position;
     savedCameraRotation = rot;
     savedCameraEuler = euler;

        if (border != null)
        {
            Image borderImage = border.GetComponent<Image>();
            if (borderImage != null)
            {
                borderImage.color = new Color(1f, 1f, 1f, 1f); // White color with full opacity
            }
        }
      currentImg = UI;
      picture = s;
        hasFaded = false;  // Reset fade state
        currentAlpha = 1f; // Reset alpha 
        DownArrow.SetActive(true);
      UpArrow.SetActive(true);
      RightArrow.SetActive(true);
      LeftArrow.SetActive(true);
      border.SetActive(true);
      toStop = false;
      //isTransitioning = true;

  }

  public void SetBorder(bool BigPic)
  {
    isBigPic = BigPic;
    Vector3 _offset = new Vector3(0, 0, 5); // Example offset
    if (!BigPic)
    {
      DownArrow.SetActive(false);
      UpArrow.SetActive(false);
      RightArrow.SetActive(false);
      LeftArrow.SetActive(false);


      border.GetComponent<Transform>().position = smallBorderPos;
      border.GetComponent<Transform>().scale = smallBorderScale;
      //border.GetComponent<Transform>().rotation = Mathf.EulertoQuat(new Vector3(mainCamera.GetComponent<Transform>().rotationEuler.X, 0, 0));
    }
    else
    {
      toStop = false;
      // UpdateObject(border, mainCamera, _offset);
      border.GetComponent<Transform>().position = bigBorderPos;
      border.GetComponent<Transform>().scale = bigBorderScale;
    }
  }

  public void ClearUI()
  {
    DownArrow.SetActive(false);
    UpArrow.SetActive(false);
    RightArrow.SetActive(false);
    LeftArrow.SetActive(false);
    border.SetActive(false);
    toStop = true;
  }

  public bool ChangeSkyBox()
  {
    return InternalCalls.SetDaySkyBox(mainCamera.mEntityID, 1.0f);
  }

  public Vector2 CalculatePitchAndYawDifferences(Quaternion current, Quaternion target)
  {
    // Compute the relative quaternion
    Quaternion relative = Quaternion.Multiply(target, Quaternion.Inverse(current));

    // Define the forward vector (-Z) and up vector (Y) in local space
    Vector3 forward = new Vector3(0.0f, 0.0f, -1.0f);
    Vector3 up = new Vector3(0.0f, 1.0f, 0.0f);

    // Rotate the forward and up vectors by the respective quaternions
    Vector3 forwardCurrent = Vector3.Transform(forward, current);
    Vector3 forwardTarget = Vector3.Transform(forward, target);
    Vector3 upCurrent = Vector3.Transform(up, current);
    Vector3 upTarget = Vector3.Transform(up, target);

    // Calculate pitch difference (angle around X-axis)
    double pitchDifference = Math.Asin(Vector3.Dot(forwardCurrent, upTarget)) * (180.0f / Math.PI);

    // Calculate yaw difference (angle around Y-axis)
    double yawDifference = Math.Atan2(forwardTarget.X, forwardTarget.Z) - Math.Atan2(forwardCurrent.X, forwardCurrent.Z);
    yawDifference *= (180.0f / Math.PI);

    // Normalize yaw to the range [-180, 180]
    if (yawDifference > 180.0f) yawDifference -= 360.0f;
    if (yawDifference < -180.0f) yawDifference += 360.0f;

    return new Vector2((float)pitchDifference, (float)yawDifference); // X = Pitch, Y = Yaw
  }

    void FadeOut()
    {
        if (currentImg != null | border != null)
        {
            isFading = true;

            currentAlpha = Mathf.Lerp(currentAlpha, 0f, fadeSpeed * Time.deltaTime);

            Color painting_color = currentImg.GetComponent<Image>().color;
            painting_color.a = currentAlpha;
            currentImg.GetComponent<Image>().color = painting_color;

            Color border_color = border.GetComponent<Image>().color;
            border_color.a = currentAlpha;
            border.GetComponent<Image>().color = border_color;

            if (Mathf.Abs(currentAlpha - 0f) < 0.01f)
            {
                currentAlpha = 0f;
                hasFaded = true;
                isFading = false;
            }
        }

    }

    //private void TransitionCamera()
    //{
    //    if (!isMovingCamera)
    //    {
    //        // Start camera transition
    //        isMovingCamera = true;
    //        elapsedTime = 0f;
    //        startY = mainCamera.GetComponent<Transform>().position.Y;
    //    }

    //    if (isMovingCamera)
    //    {
    //        elapsedTime += InternalCalls.GetDeltaTime();

    //        // Lerp the camera Y position over time
    //        float newY = Mathf.Lerp(startY, targetY, elapsedTime / moveDuration);
    //        mainCamera.GetComponent<Transform>().position = new Vector3(
    //            mainCamera.GetComponent<Transform>().position.X,
    //            newY,
    //            mainCamera.GetComponent<Transform>().position.Z
    //        );

    //        // Check if movement is complete
    //        if (elapsedTime >= moveDuration)
    //        {
    //            mainCamera.GetComponent<Transform>().position = new Vector3(
    //                mainCamera.GetComponent<Transform>().position.X,
    //                targetY,
    //                mainCamera.GetComponent<Transform>().position.Z
    //            );

    //            isMovingCamera = false; // Stop updating

    //            // Proceed with scene transition
    //            isTransitioning = false;
    //            playerMove.UnfreezePlayer();
    //            currentImg.RemoveItself();
    //            currentImg = null;
    //            InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\Level2.scn");
    //        }
    //    }
    //}
    private void TransitionCamera()
    {
        if (!isMovingCamera)
        {
            // Start camera transition
            isMovingCamera = true;
            elapsedTime = 0f;
            startY = mainCamera.GetComponent<Transform>().position.Y;

            // Trigger the fade effect before moving the camera
           
            if (corridorTransitionFadeScript != null)
            {
                Console.Write("corridorTransitionFadeScript missing");
                corridorTransitionFadeScript.isFading = true;
            }
        }

        if (isMovingCamera)
        {
            elapsedTime += InternalCalls.GetDeltaTime();

            // Lerp the camera Y position over time
            float newY = Mathf.Lerp(startY, targetY, elapsedTime / moveDuration);
            mainCamera.GetComponent<Transform>().position = new Vector3(
                mainCamera.GetComponent<Transform>().position.X,
                newY,
                mainCamera.GetComponent<Transform>().position.Z
            );

            // Check if fade is complete before transitioning scene
            if (corridorTransitionFadeScript != null && !corridorTransitionFadeScript.isFading && elapsedTime >= moveDuration)
            {
                mainCamera.GetComponent<Transform>().position = new Vector3(
                    mainCamera.GetComponent<Transform>().position.X,
                    targetY,
                    mainCamera.GetComponent<Transform>().position.Z
                );

                isMovingCamera = false; // Stop updating

                // Proceed with scene transition after fade & camera movement
                isTransitioning = false;
                playerMove.UnfreezePlayer();
                currentImg.RemoveItself();
                currentImg = null;
                InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\Level2.scn");
            }
        }
    }


}

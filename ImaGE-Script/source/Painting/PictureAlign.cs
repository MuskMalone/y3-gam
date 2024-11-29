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
  public Entity LeftClickText;
  public Entity GardenLightSpot;
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
  private Vector3 smallBorderScale = new Vector3(12.320f, 12.820f, 12.820f);
  private Vector3 bigBorderScale = new Vector3(25.980f, 27.4f, 0.38f);
  private Vector3 smallBorderPos = new Vector3(10.200f, -0.100f, -0.010f);
  private Vector3 bigBorderPos = new Vector3(0.360f, 0.4f, -0.010f);
  private Entity currentImg;
  private string picture;

  delegate void PictureInteration();

  void Start()
  {
    // Initialize the movement and camera control components
    playerMove = player.FindObjectOfType<PlayerMove>();

    if (playerMove == null) Debug.LogError("PlayerMove component not found!");
    GardenLightSpot.SetActive(true);
  }

  void Update()
  {
    if (!toStop)
    {
      // Perform alignment checks and freeze the player if aligned
      if (isBigPic && IsAligned())
      {
        if(LeftClickText != null)
          LeftClickText.SetActive(true);
        if (Input.GetMouseButtonDown(0))
        {
          toStop = true;
          alignCheck = true;
          Debug.Log("Player is aligned.");
          playerMove.FreezePlayer();
          currentImg.SetActive(false);
          isTransitioning = true;
          


          if (LeftClickText != null)
            LeftClickText.SetActive(false);
          RightArrow.SetActive(false);
          LeftArrow.SetActive(false);
          UpArrow.SetActive(false);
          DownArrow.SetActive(false);
          SetActive(false);
          InternalCalls.PlaySound(player.mEntityID, "PaintingMatchObject");
          isNight = true;
          GardenLightSpot.SetActive(false);
        }
        else
        {
          alignCheck = false;
        }
      }
      else
      {
        if (LeftClickText != null)
          LeftClickText.SetActive(false);
        alignCheck = false;
      }
    }

    else
    {
      if(isTransitioning)
      {
        //if (picture == "NightPainting")
        //{
          if(ChangeSkyBox())
          {
         
            playerMove.UnfreezePlayer();
            isTransitioning = false;
          }
        //}

      }
    }
  }


    bool IsAligned()
  {
    
    float positionDistance = Vector3.Distance(player.GetComponent<Transform>().worldPosition, savedPosition);
    Quaternion currRot = mainCamera.GetComponent<Transform>().worldRotation;
    bool aligned = true;
    //Console.WriteLine(Mathf.QuaternionAngle(currRot, savedCameraRotation));

    // Check if the main camera's rotation is within the threshold of the saved camera rotation
    if (Mathf.QuaternionAngle(currRot, savedCameraRotation) > rotationThreshold)
    {
      aligned = false;
      Vector3 curEulerAngles = Mathf.QuaternionToEuler(currRot);
      curEulerAngles.X = Mathf.RadToDeg(curEulerAngles.X);
      curEulerAngles.Y = Mathf.RadToDeg(curEulerAngles.Y);
      curEulerAngles.Z = Mathf.RadToDeg(curEulerAngles.Z);
      float pitchDifference =  Mathf.DeltaAngle(curEulerAngles.X, savedCameraEuler.X); // Up/Down
      float yawDifference = Mathf.DeltaAngle(curEulerAngles.Y, savedCameraEuler.Y);   // Left/Right
      if (Math.Abs(pitchDifference) > rotationThreshold) // Ignore small differences (optional threshold)
      {
        //Console.WriteLine($"Pitch difference: {pitchDifference}");
        if (pitchDifference > 0)
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

      // Determine yaw (left/right) adjustment
      if (Math.Abs(yawDifference) > rotationThreshold) // Ignore small differences (optional threshold)
      {
        if (yawDifference > 0)
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
    }

    if (positionDistance > positionThreshold)
    {
      aligned = false;
    }


    if (aligned)
      Console.WriteLine("Player is aligned.");
    return aligned; // All checks passed, the player is aligned
  }

  // Method to retrieve the saved camera rotation
  public Quaternion GetSavedCameraRotation()
  {
    return savedCameraRotation;
  }


  public void SetTarget(Vector3 position, Quaternion rot, Vector3 euler, string s, Entity UI)
  {
     savedPosition = position;
     savedCameraRotation = rot;
     savedCameraEuler = euler;
    currentImg = UI;
    picture = s;
    //DownArrow.SetActive(true);
    //UpArrow.SetActive(true);
    //RightArrow.SetActive(true);
    //LeftArrow.SetActive(true);
    //border.SetActive(true);
    //toStop = false;
    isTransitioning = true;
    FreezePlayer();
    SetActive(false);
  }

  public void SetBorder(bool BigPic)
  {
    isBigPic = BigPic;
    if (!BigPic)
    {
      DownArrow.SetActive(false);
      UpArrow.SetActive(false);
      RightArrow.SetActive(false);
      LeftArrow.SetActive(false);
      border.GetComponent<Transform>().position = smallBorderPos;
      border.GetComponent<Transform>().scale = smallBorderScale;
    }
    else
    {
      border.GetComponent<Transform>().position = bigBorderPos;
      border.GetComponent<Transform>().scale = bigBorderScale;
    }
  }

  public bool ChangeSkyBox()
  {
    return InternalCalls.SetDaySkyBox(mainCamera.mEntityID, 2.0f);
  }
}

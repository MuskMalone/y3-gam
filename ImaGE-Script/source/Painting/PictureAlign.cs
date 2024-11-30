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
  private HoldupUI currentImg;
  private string picture;

  delegate void PictureInteration();

  void Start()
  {
    // Initialize the movement and camera control components
    playerMove = player.FindObjectOfType<PlayerMove>();

    if (playerMove == null) Debug.LogError("PlayerMove component not found!");
  }

  void Update()
  {
    if (!toStop)
    {
      // Perform alignment checks and freeze the player if aligned
      if (isBigPic && IsAligned())
      {
        if (LeftClickText != null)
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
      if (isTransitioning)
      {
        if (picture == "NightPainting")
        {
          if (ChangeSkyBox())
          {
            InternalCalls.ChangeToolsPainting();
            playerMove.UnfreezePlayer();
            isTransitioning = false;
            currentImg.RemoveItself();
            currentImg = null;
          }
        }
        else if (picture == "ToolsPainting")
        {
          InternalCalls.SpawnToolBox();
          playerMove.UnfreezePlayer();
          isTransitioning = false;
          currentImg.RemoveItself();
          currentImg = null;
        }

      }
    }
  }


    bool IsAligned()
  {
    
    float positionDistance = Vector3.Distance(player.GetComponent<Transform>().worldPosition, savedPosition);
    Vector3 currWRot = mainCamera.GetComponent<Transform>().rotationWorldEuler;
    Vector3 currLRot = mainCamera.GetComponent<Transform>().rotationEuler;

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

    float xDiff = currLRot.X - savedCameraEuler.X;
    float yDiff = currWRot.Y - savedCameraEuler.Y;
    //Vector2 angleDiff = CalculatePitchAndYawDifferences(currWRot, savedCameraRotation);
    if (Math.Abs(xDiff) > rotationThreshold) // Ignore small differences (optional threshold)
    {

      aligned = false;
      if (xDiff < 0)
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
    //if (Math.Abs(yDiff) > rotationThreshold) // Ignore small differences (optional threshold)
    //{
    //  //Console.WriteLine(yDiff + "::" + savedCameraEuler.Y + "vs" + currWRot.Y);
    //  aligned = false;
    //  if (yDiff > 0)
    //  {
    //    //Console.WriteLine("Look left");
    //    RightArrow.SetActive(false);
    //    LeftArrow.SetActive(true);
    //  }
    //  else
    //  {
    //    //Console.WriteLine("Look right");
    //    RightArrow.SetActive(true);
    //    LeftArrow.SetActive(false);
    //  }
    //}
    //else
    //{
    //  RightArrow.SetActive(false);
    //  LeftArrow.SetActive(false);
    //}

    if (aligned)
      Console.WriteLine("Player is aligned.");
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
      currentImg = UI;
      picture = s;
      DownArrow.SetActive(true);
      UpArrow.SetActive(true);
      RightArrow.SetActive(true);
      LeftArrow.SetActive(true);
      border.SetActive(true);
      toStop = false;

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
      toStop = false;
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
    return InternalCalls.SetDaySkyBox(mainCamera.mEntityID, 2.0f);
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
}

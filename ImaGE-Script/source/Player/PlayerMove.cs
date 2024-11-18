/******************************************************************************/
/*!
\par        Image Engine
\file       .cs

\author     
\date       

\brief      


Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

using IGE.Utils;
using System.Numerics;

public class  PlayerMove : Entity
{
  public float speed = 600f;
  public float gravity = -9.81f * 2;
  public float jumpHeight = 20f;

  public Entity cam;

  private float yaw = 0f;  // Rotation around the Y-axis (horizontal, for player)
  private float pitch = 0f;  // Rotation around the X-axis (vertical, for camera)

  private float sensitivity = 0.1f;  // Mouse sensitivity
  private float maxPitch = 89f;  // Limit to prevent camera flipping (in degrees)
  private float minPitch = -89f; // Limit to prevent camera flipping (in degrees)

  private Quaternion playerRotation = Quaternion.Identity;  // Player rotation (yaw only)
  private Quaternion cameraRotation = Quaternion.Identity;  // Camera rotation (pitch only)

  // New flag to control whether the player can look around
  public bool canLook = true;
  public bool canMove = true;

  //public Vector3 velocity = new Vector3();
  bool isGrounded = true;

  public PlayerMove() : base()
  {

  }

  // Start is called before the first frame update
  void Start()
  {
    ResetPlayerVelocity();
  }

  // Update is called once per frame
  void Update()
  {
    if(InternalCalls.IsKeyTriggered(KeyCode.A))
    {
      //Console.WriteLine(GetComponent<Transform>().GetChild(0).GetChild(0).worldPosition);
    }

    if(canLook) 
      ProcessLook();
    if(canMove)
      forPlayerMovement();
  }
  void forPlayerMovement()
  {
    isGrounded = InternalCalls.IsGrounded(mEntityID);
   
      
    //if (isGrounded && velocity.Y < 0)
    //{
    //  velocity.Y = -2f;
    //}


    float x = Input.GetAxis("Horizontal");
    float z = Input.GetAxis("Vertical");


    Vector3 move = GetComponent<Transform>().right * x *speed + GetComponent<Transform>().forward * z * speed;


      

    //check if the player is on the ground so he can jump
    if (Input.GetKeyDown(KeyCode.SPACE) && isGrounded)
    {
      move.Y = jumpHeight;  
    }

    InternalCalls.MoveCharacter(mEntityID, move);
  }

  void ProcessLook()
  {
    if (!canLook) return;  // Skip look processing if the player is frozen
    Vector3 mouseDelt = InternalCalls.GetMouseDelta();
    float mouseDeltaX = mouseDelt.X;
    float mouseDeltaY = mouseDelt.Y;
    //Console.WriteLine(mouseDeltaX);
    yaw -= mouseDeltaX * sensitivity;
    //Console.WriteLine(yaw);

    // Apply mouse delta to pitch (rotate camera around the X-axis)
    pitch -= mouseDeltaY * sensitivity;

    // Clamp pitch to prevent camera from flipping upside down
    pitch = Mathf.Clamp(pitch, minPitch, maxPitch);
    //Console.WriteLine(pitch);

    // Update the camera's rotation (pitch)
    cam.GetComponent<Transform>().rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitX, Mathf.DegToRad(pitch));

    // Update the player's rotation (yaw)
    GetComponent<Transform>().worldRotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, Mathf.DegToRad(yaw));
  }

  private void ResetPlayerVelocity()
  {
    InternalCalls.MoveCharacter(mEntityID, new Vector3(0, 0, 0));
    InternalCalls.SetAngularVelocity(mEntityID, new Vector3(0, 0, 0));
  }

  // Called by other scripts to Freeze/Unfreeze Player
  public void FreezePlayer()
  {
    ResetPlayerVelocity();
    canMove = false;
    canLook = false;
  }

  public void UnfreezePlayer()
  {
    ResetPlayerVelocity();
    canMove = true;
    canLook = true;
  }
}
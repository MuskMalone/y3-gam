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
using System;
using System.Numerics;

public class  PlayerMove : Entity
{
  public float speed = 650f;
  public float jumpForce = 2500f;
  private float extraGravityFactorDuringDescent = 15f;
  public float isGroundedRayHeight = 3f;
  public Entity cam;

  private float yaw = 0f;                                   // Rotation around the Y-axis (horizontal, for player)
  private float pitch = 0f;                                 // Rotation around the X-axis (vertical, for camera)

  private float sensitivity = 0.1f;                         // Mouse sensitivity
  private float maxPitch = 89f;                             // Limit to prevent camera flipping (in degrees)
  private float minPitch = -89f;                            // Limit to prevent camera flipping (in degrees)

  private Quaternion playerRotation = Quaternion.Identity;  // Player rotation (yaw only)
  private Quaternion cameraRotation = Quaternion.Identity;  // Camera rotation (pitch only)
  private float initialGravityFactor = 5f;

  public bool canLook = true;
  public bool canMove = true;

  public PlayerMove() : base()
  {

  }

  // Start is called before the first frame update
  void Start()
  {
    ResetPlayerVelocity();
    //initialGravityFactor = InternalCalls.GetGravityFactor(mEntityID);
  }

  // Update is called once per frame
  void Update()
  {
    if(canLook) 
      ProcessLook();
    if(canMove)
      PlayerMovement();
  }
  void PlayerMovement()
  {
    float x = Input.GetAxis("Horizontal");
    float z = Input.GetAxis("Vertical");

    Vector3 move = GetComponent<Transform>().right * x * speed + GetComponent<Transform>().forward * z * speed;

    InternalCalls.MoveCharacter(mEntityID, move);

    if (Input.GetKeyTriggered(KeyCode.SPACE) && IsGrounded())
    {
      Jump();
    }

    if (IsGrounded())
    {
      InternalCalls.SetGravityFactor(mEntityID, initialGravityFactor);
    }

    else
    {
      InternalCalls.SetGravityFactor(mEntityID, initialGravityFactor * extraGravityFactorDuringDescent);
    }
  }

  void ProcessLook()
  {
    if (!canLook) return;  // Skip look processing if the player is frozen
    Vector3 mouseDelt = InternalCalls.GetMouseDelta();
    float mouseDeltaX = mouseDelt.X;
    float mouseDeltaY = mouseDelt.Y;

    yaw -= mouseDeltaX * sensitivity;

    // Apply mouse delta to pitch (rotate camera around the X-axis)
    pitch -= mouseDeltaY * sensitivity;

    // Clamp pitch to prevent camera from flipping upside down
    pitch = Mathf.Clamp(pitch, minPitch, maxPitch);

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

  private void Jump()
  {
    Vector3 currentVelocity = InternalCalls.GetVelocity(mEntityID);
    currentVelocity.Y = jumpForce;
    InternalCalls.SetVelocity(mEntityID, currentVelocity);
  }

  public bool IsGrounded()
  {
    Vector3 entityPosition = InternalCalls.GetWorldPosition(mEntityID);
    Vector3 rayStart = entityPosition;
    // Ray ends slightly beneath the entity
    Vector3 rayEnd = entityPosition + new Vector3(0, 0 - isGroundedRayHeight, 0);
    uint entityIDHit = InternalCalls.RaycastFromEntity(mEntityID, rayStart, rayEnd);
    return entityIDHit != 0;
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
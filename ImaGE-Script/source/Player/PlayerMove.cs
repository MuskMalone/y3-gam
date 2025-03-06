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
using System.Text;

public class  PlayerMove : Entity
{
  public float speed = 750f;
  public float walkingSpeed = 750f;
  public float runSpeed = 1300f;
  public float isGroundedRayHeight = 3f;
  public Entity cam;

  private float yaw = 0f;                                   // Rotation around the Y-axis (horizontal, for player)
  private float pitch = 0f;                                 // Rotation around the X-axis (vertical, for camera)

  private float sensitivity = 0.1f;                         // Mouse sensitivity
  private float maxPitch = 89f;                             // Limit to prevent camera flipping (in degrees)
  private float minPitch = -89f;                            // Limit to prevent camera flipping (in degrees)

  private Quaternion playerRotation = Quaternion.Identity;  // Player rotation (yaw only)
  private Quaternion cameraRotation = Quaternion.Identity;  // Camera rotation (pitch only)
  public float initialGravityFactor = 5f;
  public float extraGravityFactorDuringDescent = 15f;

  public bool canLook = true, canMove = true;
  private bool skipNextMouseDelta = false;  // to skip the jump in delta when unfreezing player
  private double currTime = 0.0;
  private double targetTime = 1.0;
  private bool startTimer = false;

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
 
    if (startTimer)
    {
      currTime += InternalCalls.GetDeltaTime();
      if (currTime > targetTime)
      {
        UnfreezePlayer();
        startTimer = false;
      }
    }

    // Skip look processing if the player is frozen
    if (canLook)
      ProcessLook();

    if (canMove)
      PlayerMovement();
  }
  void PlayerMovement()
  {
    float x = Input.GetAxis("Horizontal");
    float z = Input.GetAxis("Vertical");

    if (Input.GetKeyDown(KeyCode.Q))
    {
      speed = runSpeed;
    }

    else
    {
      speed = walkingSpeed;
    }
    Vector3 movementVector = (GetComponent<Transform>().right * x + GetComponent<Transform>().forward * z);
    float movementVectorLength = movementVector.Length();
    if (Math.Abs(movementVectorLength) > float.Epsilon)
    {
      movementVector /= movementVectorLength;
    }
    Vector3 move = movementVector * speed;

    InternalCalls.MoveCharacter(mEntityID, move);

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
    Vector3 mouseDelt = InternalCalls.GetMouseDelta();

    if (skipNextMouseDelta && mouseDelt.X != 0.0f && mouseDelt.Y != 0.0f) {
      skipNextMouseDelta = false;
      //Debug.Log("Skipped delta of: " + InternalCalls.GetMouseDelta().ToString());
      return;
    }

    float mouseDeltaX = mouseDelt.X;
    float mouseDeltaY = mouseDelt.Y;

    yaw -= mouseDeltaX * sensitivity;
    if (yaw > 360.0f || yaw < -360.0f)
      yaw /= 360.0f;

    // Apply mouse delta to pitch (rotate camera around the X-axis)
    pitch -= mouseDeltaY * sensitivity;

    // Clamp pitch to prevent camera from flipping upside down
    pitch = Mathf.Clamp(pitch, minPitch, maxPitch);

    // Console.WriteLine(yaw);
    // Update the camera's rotation (pitch)
    cam.GetComponent<Transform>().rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitX, Mathf.DegToRad(pitch));

    // Update the player's rotation (yaw)
    GetComponent<Transform>().rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, Mathf.DegToRad(yaw));
    // not sure if theres another way
    InternalCalls.UpdatePhysicsToTransform(mEntityID);  // fix for player physics overriding transform updates
  }

  private void ResetPlayerVelocity()
  {
    InternalCalls.MoveCharacter(mEntityID, new Vector3(0, 0, 0));
    InternalCalls.SetAngularVelocity(mEntityID, new Vector3(0, 0, 0));
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
    skipNextMouseDelta = true;
  }


  public void SetUnfreeTimer()
  {
    currTime = 0.0;
    startTimer = true;
  }

  public Vector2 GetRotation()
  {
    return new Vector2(pitch,yaw);
  }
}
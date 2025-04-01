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
#define COMMENT_OUT_FOR_SUBMISSION

using IGE.Utils;
using System;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Text;

public class  PlayerMove : Entity
{
  public float speed = 750f;
  public float walkingSpeed = 750f;
  public float runSpeed = 1300f;
  public bool noClip = false;
  public bool isTutorial = false;
  public Entity cam;

  public float yaw = 0f;                                   // Rotation around the Y-axis (horizontal, for player)
  public float pitch = 0f;                                 // Rotation around the X-axis (vertical, for camera)

  public float sensitivity = 0.1f;                         // Mouse sensitivity
  private float maxPitch = 89f;                             // Limit to prevent camera flipping (in degrees)
  private float minPitch = -89f;                            // Limit to prevent camera flipping (in degrees)
  
  private Quaternion playerRotation = Quaternion.Identity;  // Player rotation (yaw only)
  private Quaternion cameraRotation = Quaternion.Identity;  // Camera rotation (pitch only)
  public bool canLook = true, canMove = true, useScriptRotation = true, climbing = false;
  private bool skipNextMouseDelta = false;  // to skip the jump in delta when unfreezing player
  private double currTime = 0.0;
  private double targetTime = 1.0;
  private bool startTimer = false;

  private readonly float defaultGravityFactor = 0f;
  private readonly float inAirGravityFactor = 40f;
  private bool ungrounded = false;

  public float isGroundedRayHeight = 3f;
  public float minIsGroundedDistance = 4.35f;

  public PlayerMove() : base()
  {

  }

  // Start is called before the first frame update
  void Start()
  {
    ResetPlayerVelocity();
    InternalCalls.SetGravityFactor(mEntityID, defaultGravityFactor);
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
    if(useScriptRotation)
    {
      ProcessLook();
    }
    

    if (canMove)
      PlayerMovement();
  }

  public void ResetGravityFactor()
  {
    ungrounded = true;
    InternalCalls.SetGravityFactor(mEntityID, inAirGravityFactor);
  }

  void PlayerMovement()
  {
#if COMMENT_OUT_FOR_SUBMISSION
    // tilde(~) key to ALLOW NOCLIP MODE TO FLY AROUND
    // CONTROLS: WASD LSHIFT SPACE
    if (Input.GetKeyTriggered(KeyCode.GRAVE_ACCENT))
    {
      noClip = !noClip;
      if (noClip)
      {
        InternalCalls.SetGravityFactor(mEntityID, 0f);
      }
      else
      {
        ResetGravityFactor();
      }
    }
#endif

    if (ungrounded)
    {
      if (IsGrounded())
      {
        InternalCalls.SetGravityFactor(mEntityID, defaultGravityFactor);
        ungrounded = false;
      }
    }

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

#if COMMENT_OUT_FOR_SUBMISSION
    if (noClip)
    {
      if (Input.GetKeyDown(KeyCode.SPACE))
      {
        movementVector.Y += 1f;
      }
      if (Input.GetKeyDown(KeyCode.LEFT_SHIFT))
      {
        movementVector.Y -= 1f;
      }
      GetComponent<Transform>().worldPosition += movementVector * (Input.GetKeyDown(KeyCode.Q) ? 5f : 3f);
      InternalCalls.UpdatePhysicsToTransform(mEntityID);
    }
    else
#endif
    {
      InternalCalls.MoveCharacter(mEntityID, move);
    }
  }

  void ProcessLook()
  {
    if (canLook)
    {
      Vector3 mouseDelt = InternalCalls.GetMouseDelta();

      if (skipNextMouseDelta && mouseDelt.X != 0.0f && mouseDelt.Y != 0.0f)
      {
        skipNextMouseDelta = false;
        //Debug.Log("Skipped delta of: " + InternalCalls.GetMouseDelta().ToString());
        return;
      }

      float mouseDeltaX = mouseDelt.X;
      float mouseDeltaY = mouseDelt.Y;

      yaw -= mouseDeltaX * sensitivity;

      // Apply mouse delta to pitch (rotate camera around the X-axis)
      pitch -= mouseDeltaY * sensitivity;
    }

    if (yaw > 360.0f || yaw < -360.0f)
      yaw /= 360.0f;

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
    RaycastHitInfo hitInfo = InternalCalls.RaycastFromEntityInfo(mEntityID, rayStart, rayEnd);

    uint entityIDHit = InternalCalls.RaycastFromEntity(mEntityID, rayStart, rayEnd);
    //Console.WriteLine($"distance {hitInfo.distance} tag {InternalCalls.GetTag(entityIDHit)} >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    //return entityIDHit != uint.MaxValue;
    return hitInfo.distance < minIsGroundedDistance;
    //return true;
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

  public void SetRotation(Vector3 rot)
  {
    yaw = rot.Y;                                   // Rotation around the Y-axis (horizontal, for player)
    pitch = rot.X;                                 // Rotation around the X-axis (vertical, for camera)
}


}
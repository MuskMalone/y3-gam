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

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
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

  public Vector3 velocity = new Vector3();
  bool isGrounded = true;
  public PlayerMove() : base()
  {

    //AddComponent<Transform>(new Transform());
  }


  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    if(InternalCalls.IsKeyTriggered(KeyCode.A))
    {
      //SetActive(false);
    }

    ProcessLook();
    forPlayerMovement();


  }
    void forPlayerMovement()
    {
        //for testing

      isGrounded = InternalCalls.IsGrounded(mEntityID);

      if (isGrounded && velocity.Y < 0)
      {
        velocity.Y = -2f;
      }


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
}



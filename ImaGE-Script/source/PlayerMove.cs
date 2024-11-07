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
using Image.Utils;
using System.Numerics;

public class  PlayerMove : Entity
{
  public float speed = 12f;
  public float gravity = -9.81f * 2;
  public float jumpHeight = 3f;


  Vector3 velocity;
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
    //HandleBotSpawningAndSwitching();

    forPlayerMovement();

  }
    void forPlayerMovement()
    {

     // isGrounded = InternalCalls.IsGrounded(mEntityID);

      if (isGrounded && velocity.Y < 0)
      {
        velocity.Y = -2f;
      }


      float x = Input.GetAxis("Horizontal");
      float z = Input.GetAxis("Vertical");
      //Console.WriteLine(z);
      Console.WriteLine(x);

    //right is the red Axis, foward is the blue axis
    Vector3 move = GetComponent<Transform>().right * x + GetComponent<Transform>().forward * z;

      InternalCalls.MoveCharacter(mEntityID,move * speed * Time.deltaTime);

      //check if the player is on the ground so he can jump
      if (Input.GetKeyDown(KeyCode.SPACE) && isGrounded)
      {
        //the equation for jumping
        velocity.Y = Mathf.Sqrt(jumpHeight * -2f * gravity);
      }
      
     // velocity.Y += gravity * Time.deltaTime;

      InternalCalls.MoveCharacter(mEntityID, velocity * Time.deltaTime);

  }
  }



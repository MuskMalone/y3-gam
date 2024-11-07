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

using Image.Mono.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Numerics;

namespace Image.Mono
{
  public class PlayerMovement : Entity
  {
    public CharacterController controller;

    public float speed = 12f;
    public float gravity = -9.81f * 2;
    public float jumpHeight = 3f;

    public Transform groundCheck;
    public float groundDistance = 0.4f;
    public LayerMask groundMask;



    Vector3 velocity;
    bool isGrounded;

    // Update is called once per frame
    void Update()
    {
      //HandleBotSpawningAndSwitching();

      forPlayerMovement();

    }



    void forPlayerMovement()
    {
      //checking if we hit the ground to reset our falling velocity, otherwise we will fall faster the next time
      isGrounded = Physics.CheckSphere(groundCheck.position, groundDistance, groundMask);

      if (isGrounded && velocity.y < 0)
      {
        velocity.y = -2f;
      }

      float x = Input.GetAxis("Horizontal");
      float z = Input.GetAxis("Vertical");

      //right is the red Axis, foward is the blue axis
      Vector3 move = transform.right * x + transform.forward * z;

      controller.Move(move * speed * Time.deltaTime);

      //check if the player is on the ground so he can jump
      if (Input.GetButtonDown("Jump") && isGrounded)
      {
        //the equation for jumping
        velocity.y = Mathf.Sqrt(jumpHeight * -2f * gravity);
      }

      velocity.y += gravity * Time.deltaTime;

      controller.Move(velocity * Time.deltaTime);
    }
  }

}

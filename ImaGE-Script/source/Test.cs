/*!*********************************************************************
\file         Stats.cs
\author       chengen.lau\@digipen.edu
\co-author    c.phua\@digipen.edu, han.q\@digipen.edu
\co-author    Han Qin Ding
\date         10-January-2024
\brief        Calculates and keep tracks of stats related to a
              character. Mostly contains instances of elements
              required to run the game such as the deck, combo, buff
              manager, health, attack, block and ui elements to
              update. These are managed by the GameManager to
              update the relevant values based on the current game
              phase.
 
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Image.Mono;

namespace Image
{



public class Test : Entity
{

    private float speed = 20f; 

    // VARIABLES HERE SHOULD ONLY BE MODFIED THROUGH EDITOR
    public int TestInt = 10;
    public float TestFloat = 20f;
    public double TestDouble = 30.0;
    public Vec3<double> dVec3 = new Vec3<double>(336.318f, 100f, 0f);

    

    public Test(uint entityID) : base(entityID)
    {
      Console.WriteLine("ENTITYID" + entityID);
    }

    /*!*********************************************************************
    \brief
      Initializes deck and combo managers based on the character type.
    ************************************************************************/
    public void OnCreate()
    {

    }

    /*!*********************************************************************
    \brief  
      Initializes character's hand with the correct number of cards
    ************************************************************************/
    public void Init()
    {
    }

    /*!*********************************************************************
    \brief  
      Update function for the class. Takes in the deltaTime from GameManager
      and updates the relevant members with it.
    ************************************************************************/
    public void Update(double dt)
    {
     // Console.WriteLine("Hit\n");
      Vec3<float> currTrans = InternalCalls.GetPosition(mEntityID);
      bool isChanged = false;

      currTrans.Y += (InternalCalls.IsKeyTriggered(KeyCode.W) || InternalCalls.IsKeyHeld(KeyCode.W)) ? (speed * (float)dt) : 0;
      currTrans.X -= (InternalCalls.IsKeyTriggered(KeyCode.A) || InternalCalls.IsKeyHeld(KeyCode.A)) ? (speed * (float)dt) : 0;
      currTrans.Y -= (InternalCalls.IsKeyTriggered(KeyCode.S) || InternalCalls.IsKeyHeld(KeyCode.S)) ? (speed * (float)dt) : 0;
      currTrans.X += (InternalCalls.IsKeyTriggered(KeyCode.D) || InternalCalls.IsKeyHeld(KeyCode.D)) ? (speed * (float)dt) : 0;

      isChanged =
          (InternalCalls.IsKeyTriggered(KeyCode.W) || InternalCalls.IsKeyHeld(KeyCode.W)) ||
          (InternalCalls.IsKeyTriggered(KeyCode.A) || InternalCalls.IsKeyHeld(KeyCode.A)) ||
          (InternalCalls.IsKeyTriggered(KeyCode.S) || InternalCalls.IsKeyHeld(KeyCode.S)) ||
          (InternalCalls.IsKeyTriggered(KeyCode.D) || InternalCalls.IsKeyHeld(KeyCode.D));

    if(isChanged)
      {
        InternalCalls.SetPosition(mEntityID, ref currTrans);
      }
    


  }


   
  }
}


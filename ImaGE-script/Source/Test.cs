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
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;



public class Test : Entity
{

  //  private float speed = 20f;

  //// VARIABLES HERE SHOULD ONLY BE MODFIED THROUGH EDITOR
  //  public Entity testEnt;

  public int TestInt = 10;
  //public float TestFloat = 22f;
  //public double TestDouble = 30.0;
  //public Vector3 dVec3 = new Vector3(336.318f, 100f, 0f);





  public Test() : base()
    {
      
      //AddComponent<Transform>(new Transform());
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
    public void Update()
    {

    //Console.WriteLine("TEST INT:" + TestInt);
      //  Console.WriteLine(testEnt.mEntityID.ToString());
      // Console.WriteLine("Hit\n");
      //if (testEnt != null)
      //{
      //  Console.WriteLine(testEnt.mEntityID + "::ID");
      //}
      //Vec3<float> Position = GetComponent<Transform>().Position;
      //GetComponent<Transform>().Position = new Vec3<float>(1,1,2);
      //  Console.WriteLine(Position.X + "," + Position.Y + "," + Position.Z);

    //  bool isChanged = false;

    //  currTrans.Y += (InternalCalls.IsKeyTriggered(KeyCode.W) || InternalCalls.IsKeyHeld(KeyCode.W)) ? (speed * (float)dt) : 0;
    //  currTrans.X -= (InternalCalls.IsKeyTriggered(KeyCode.A) || InternalCalls.IsKeyHeld(KeyCode.A)) ? (speed * (float)dt) : 0;
    //  currTrans.Y -= (InternalCalls.IsKeyTriggered(KeyCode.S) || InternalCalls.IsKeyHeld(KeyCode.S)) ? (speed * (float)dt) : 0;
    //  currTrans.X += (InternalCalls.IsKeyTriggered(KeyCode.D) || InternalCalls.IsKeyHeld(KeyCode.D)) ? (speed * (float)dt) : 0;


    //  isChanged =
    //      (InternalCalls.IsKeyTriggered(KeyCode.W) || InternalCalls.IsKeyHeld(KeyCode.W)) ||
    //      (InternalCalls.IsKeyTriggered(KeyCode.A) || InternalCalls.IsKeyHeld(KeyCode.A)) ||
    //      (InternalCalls.IsKeyTriggered(KeyCode.S) || InternalCalls.IsKeyHeld(KeyCode.S)) ||
    //      (InternalCalls.IsKeyTriggered(KeyCode.D) || InternalCalls.IsKeyHeld(KeyCode.D));

    //if(isChanged)
    //  {
    //    InternalCalls.SetPosition(mEntityID, ref currTrans);
    //  }



    }

    public void OnPointerEnter() {
        string tag = InternalCalls.GetTag(mEntityID);
        Console.WriteLine("From MONO Enter" + tag);
    }

    public void OnPointerExit()
    {
        string tag = InternalCalls.GetTag(mEntityID);
        Console.WriteLine("From MONO Exit" + tag);
    }

    public void OnPointerDown()
    {
        string tag = InternalCalls.GetTag(mEntityID);
        Console.WriteLine("From MONO Down" + tag);
    }

    public void OnPointerUp()
    {
        string tag = InternalCalls.GetTag(mEntityID);
        Console.WriteLine("From MONO Up" + tag);
    }

}



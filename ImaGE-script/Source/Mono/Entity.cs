/******************************************************************************/
/*!
\par        Image Engine
\file       Main.cs

\author     Han Qin Ding(han.q@digipen.edu), Ernest Cheo (e.cheo@digipen.edu)
\date       28 September, 2024

\brief      
  The main entity class is located here and has the getter setters 
  for all the required rigid body variables. Helper functions also
  located here.

Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using System.Diagnostics;
using IGE.Utils;


  public class Entity
  {

    public uint mEntityID;
    //private readonly Dictionary<Type, Component> components = new Dictionary<Type, Component>();


    /*  _________________________________________________________________________ */
    /*! Entity

    @return *this

    Default constructor for entity.
    */
    public Entity()
    {
      //mEntityID = uint.MaxValue;
    }

    public void Init(uint entityID)
    {
      mEntityID = entityID;
      Console.WriteLine("Init ID: " + mEntityID);
    }


   // Method to add a component

   // Generic GetComponent method
   public T GetComponent<T>() where T : Component, new()
    {
      if (mEntityID == null)
      {
        Debug.LogError("Invalid EntityID");
        return null;
      }
      // Try to retrieve the component of type T
      T component = new T() { entity = this };
      return component;
    }

    public void SetActive(bool active)
    {
      InternalCalls.SetActive(mEntityID, active);
    }


    
  }


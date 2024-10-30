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
using System.Diagnostics;



namespace Image.Mono
{

    public class Entity
  {

    public int mEntityID = 666;
    //private readonly Dictionary<Type, Component> components = new Dictionary<Type, Component>();

    /*  _________________________________________________________________________ */
    /*! Entity

    @param entityHandle
    The mEntityID .

    @return *this

    Non-default, single-arg constructor for entity
    */
    public Entity(int entityHandle)
    {
      mEntityID = entityHandle;
    }

    /*  _________________________________________________________________________ */
    /*! Entity

    @return *this

    Default constructor for entity.
    */
    public Entity()
    {
      mEntityID  = 0;
    }
      


    // Method to add a component

    // Generic GetComponent method
    public T GetComponent<T>() where T : Component, new()
    {
      if (mEntityID == Utils.InvalidID)
      {
        InternalCalls.LogError("Invalid EntityID");
        return null;
      }
      // Try to retrieve the component of type T
      T component = new T() { entity = this };
      return component;
    }


    
  }
}

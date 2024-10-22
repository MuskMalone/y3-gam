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

namespace Image.Mono
{
  public class Entity
  {
    public uint mEntityID;
    private readonly Dictionary<Type, Component> components = new Dictionary<Type, Component>();

    /*  _________________________________________________________________________ */
    /*! Entity

    @param entityHandle
    The mEntityID .

    @return *this

    Non-default, single-arg constructor for entity
    */
    public Entity(uint entityHandle)
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
    public void AddComponent<T>(T component) where T : Component
    {
        components[typeof(T)] = component;
    }

    // Generic GetComponent method
    public T GetComponent<T>() where T : Component
    {
        // Try to retrieve the component of type T
        if (components.TryGetValue(typeof(T), out var component))
        {
            return component as T; // Cast to type T
        }

        return null; // Return null if component not found
    }


    
  }
}

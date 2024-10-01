/******************************************************************************/
/*!
\par        Image Engine
\file       Main.cs

\author     Han Qin Ding(han.q@digipen.edu), Ernest Cheo (e.cheo@digipen.edu)
\date       28 September, 2024

\brief      The main entity class is located here and has the getter setters 
            for all the required rigid body variables. Helper functions also
            located here.

\copyright  Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
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

namespace Image
{
  public class Entity
  {
    public uint entityID;

    /*  _________________________________________________________________________ */
    /*! Entity

    @param entityHandle
    The entityID.

    @return *this

    Non-default, single-arg constructor for entity
    */
    public Entity(uint entityHandle)
    {
      entityID = entityHandle;
    }

    /*  _________________________________________________________________________ */
    /*! Entity

    @return *this

    Default constructor for entity.
    */
    public Entity()
    {
      entityID = 0;
    }

    /*  _________________________________________________________________________ */
    /*! As

    @return T

    For turning any generic Entity into its respective child class.
    */
    public T As<T>() where T : Entity, new()
    {
      object instance = InternalCalls.EngineCore_GetScriptInstance(ref entityID);
      return instance as T;
    }

    #region Graphics
    public Vector2 CameraPosition
    {
      get
      {
        Vector2 pos = new Vector2(0, 0);
        InternalCalls.GraphicsComponent_GetCamPosition(ref pos);
        return pos;
      }

      set
      {
        InternalCalls.GraphicsComponent_SetCamPosition(ref value);
      }
    }
    /*  _________________________________________________________________________ */
    /*! AnimationState

    Getter setter for AnimationState.
    */
    public int AnimationState
    {
      get
      {
        int animationState = 0;
        InternalCalls.AnimationComponent_GetAnimationState(ref entityID, ref animationState);
        return AnimationState;
      }
      set
      {
        InternalCalls.AnimationComponent_SetAnimationState(ref entityID, ref value);
      }
    }

    /*  _________________________________________________________________________ */
    /*! SetSprite

    Wrapper function for setting the sprite based on the sprite's filename.
    */
    public void SetSprite(string fileName)
    {
      InternalCalls.GraphicsComponent_SetSprite(ref entityID, fileName);
    }

    /*  _________________________________________________________________________ */
    /*! Scale

    Getter setter for Scale.
    */
    public Vector3 Scale
    {
      get
      {
        Vector3 scale = new Vector3();
        InternalCalls.GraphicsComponent_GetScale(ref entityID, ref scale);
        return scale;
      }
      set
      {
        InternalCalls.GraphicsComponent_SetScale(ref entityID, ref value);
      }
    }

    /*  _________________________________________________________________________ */
    /*! Rotation

    Getter setter for Rotation.
    */
    public Vector3 Rotation
    {
      get
      {
        Vector3 rotation = new Vector3();
        InternalCalls.GraphicsComponent_GetRotation(ref entityID, ref rotation);
        return rotation;
      }
      set
      {
        InternalCalls.GraphicsComponent_SetRotation(ref entityID, ref value);
      }
    }

    /*  _________________________________________________________________________ */
    /*! GetScaleFromEntity

    Get the scale, given the entity id.
    */
    public Vector3 GetScaleFromEntity(uint id)
    {
      Vector3 scale = new Vector3();
      InternalCalls.GraphicsComponent_GetScale(ref id, ref scale);
      return scale;
    }

    /*  _________________________________________________________________________ */
    /*! SetScaleFromEntity

    Set the scale, given the entity id.
    */
    public void SetScaleFromEntity(uint id, Vector3 value)
    {
      InternalCalls.GraphicsComponent_SetScale(ref id, ref value);
    }
    #endregion

    #region Physics
    /*  _________________________________________________________________________ */
    /*! Translation

    Getter setter for Translation.
    */
    public Vec3<double> Translation
    {
      get
      {
        Vec3<double> translation = new Vec3<double>();
        InternalCalls.TransformComponent_GetTranslation(ref entityID, ref translation);
        return translation;
      }
      set
      {
        InternalCalls.TransformComponent_SetTranslation(ref entityID, ref value);
      }
    }
    public float Transform_Rotation
    {
      get
      {
        float rot = 0;
        InternalCalls.TransformComponent_GetRotation(ref entityID, ref rot);
        return rot;
      }
      set
      {
        InternalCalls.TransformComponent_SetRotation(ref entityID, ref value);
      }
    }
    #endregion
  }
}

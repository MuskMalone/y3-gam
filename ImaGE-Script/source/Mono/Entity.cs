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

namespace Image.Mono
{
  public class Entity
  {
    public uint mEntityID;

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

    /*  _________________________________________________________________________ */
    /*! As

    @return T

    For turning any generic Entity into its respective child class.
    */
    //public T As<T>() where T : Entity, new()
    //{
    //  object instance = InternalCalls.EngineCore_GetScriptInstance(ref mEntityID );
    //  return instance as T;
    //}

    //#region Graphics
    //public Vector2 CameraPosition
    //{
    //  get
    //  {
    //    Vector2 pos = new Vector2(0, 0);
    //    InternalCalls.GraphicsComponent_GetCamPosition(ref pos);
    //    return pos;
    //  }

    //  set
    //  {
    //    InternalCalls.GraphicsComponent_SetCamPosition(ref value);
    //  }
    //}
    /*  _________________________________________________________________________ */
    /*! AnimationState

    Getter setter for AnimationState.
    */
    //public int AnimationState
    //{
    //  get
    //  {
    //    int animationState = 0;
    //    InternalCalls.AnimationComponent_GetAnimationState(ref mEntityID , ref animationState);
    //    return AnimationState;
    //  }
    //  set
    //  {
    //    InternalCalls.AnimationComponent_SetAnimationState(ref mEntityID , ref value);
    //  }
    //}

    /*  _________________________________________________________________________ */
    /*! SetSprite

    Wrapper function for setting the sprite based on the sprite's filename.
    */
    //public void SetSprite(string fileName)
    //{
    //  InternalCalls.GraphicsComponent_SetSprite(ref mEntityID , fileName);
    //}


    #region Transform

    /*  _________________________________________________________________________ */
    /*! Scale

    Getter setter for Scale.
    */
    public Vec3<float> Scale
    {
      get
      {
        Vec3<float> scale = new Vec3<float>();
        InternalCalls.GetWorldScale(mEntityID);
        return scale;
      }
      set
      {
        InternalCalls.SetWorldScale(ref mEntityID , ref value);
      }
    }


    /*  _________________________________________________________________________ */
    /*! GetScaleFromEntity

    Get the scale, given the entity id.
    */
    public Vec3<float> GetScaleFromEntity(uint id)
    {
      Vec3<float> scale = new Vec3<float>();
      InternalCalls.GetWorldScale(id);
      return scale;
    }

    /*  _________________________________________________________________________ */
    /*! SetScaleFromEntity

    Set the scale, given the entity id.
    */
    public void SetScaleFromEntity(uint id, Vec3<float> value)
    {
      InternalCalls.SetWorldScale(ref id, ref value);
    }



    /*  _________________________________________________________________________ */
    /*! Translation

    Getter setter for Translation.
    */
    public Vec3<float> Position
    {
      get
      {
        Vec3<float> translation = new Vec3<float>();
        InternalCalls.GetPosition(mEntityID);
        return translation;
      }
      set
      {
        InternalCalls.SetPosition(mEntityID , ref value);
      }
    }
    //public float Transform_Rotation
    //{
    //  get
    //  {
    //    float rot = 0;
    //    InternalCalls.TransformComponent_GetRotation(ref mEntityID , ref rot);
    //    return rot;
    //  }
    //  set
    //  {
    //    InternalCalls.TransformComponent_SetRotation(ref mEntityID , ref value);
    //  }
    //}
    #endregion
  }
}

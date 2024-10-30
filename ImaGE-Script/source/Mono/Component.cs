using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;

namespace Image.Mono
{
  
  public abstract class Component
  {
    // Base class for all components
    public Entity entity { get; internal set; }
  }


  #region Transform

  public class Transform : Component
  {

    public Vec3<float> position
    {
      get
      {
        return InternalCalls.GetPosition(entity.mEntityID); // Push update to C++ side
      }
      set
      {
        InternalCalls.SetPosition(entity.mEntityID, ref value); // Push update to C++ side
      }
    }

    //public Quaternion Rotation
    //{
    //  get => mRotation;
    //  set
    //  {
    //    mRotation = value;
    //    UpdateCPlusPlusTransform(); // Push update to C++ side
    //  }
    //}

    //public Vec3<float> Scale
    //{
    //  get => mScale;
    //  set
    //  {
    //    mScale = value;
    //    UpdateCPlusPlusTransform(); // Push update to C++ side
    //  }
    //}

    public Vec3<float> scale
    {
      get
      {
        return InternalCalls.GetScale(entity.mEntityID); // Push update to C++ side
      }
      set
      {
        InternalCalls.SetScale(entity.mEntityID, ref value); // Push update to C++ side
      }
    }
  }
  #endregion

    public class Tag : Component
    {

      public string tag
      {
        get
        {
          return InternalCalls.GetTag(entity.mEntityID); // Push update to C++ side
        }
    }
    }
  }

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
    public uint mEntityID { get; internal set; }
  }


  //#region Transform

  //public class Transform : Component
  //{
  //  // Implement collider-specific logic
  //  private Vec3<float> mPosition;
  //  private Vec3<float> mScale;
  //  private Quaternion rotation;

  //  public Vec3<float> Position
  //  {
  //    get => mPosition;
  //    set
  //    {
  //      mPosition = value;
  //      InternalCalls.SetPosition(mEntityID,mPosition); // Push update to C++ side
  //    }
  //  }

  //  public Quaternion Rotation
  //  {
  //    get => mRotation;
  //    set
  //    {
  //      mRotation = value;
  //      UpdateCPlusPlusTransform(); // Push update to C++ side
  //    }
  //  }

  //  public Vec3<float> Scale
  //  {
  //    get => mScale;
  //    set
  //    {
  //      mScale = value;
  //      UpdateCPlusPlusTransform(); // Push update to C++ side
  //    }
  //  }
  //}
  //#endregion

}

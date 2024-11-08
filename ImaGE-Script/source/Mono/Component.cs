using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;


  
  public class Component
  {
    // Base class for all components
    public Entity entity { get; internal set; }
  }


  #region Transform

  public class Transform : Component
  {

    public Vector3 position
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

    public Quaternion rotation
    {
      get
      {
        return InternalCalls.GetRotation(entity.mEntityID) ; // Push update to C++ side
      }
      set
      {
        InternalCalls.SetRotation(entity.mEntityID, ref value); // Push update to C++ side
      }
    }

    public Quaternion worldRotation
    {
      get
      {
        return InternalCalls.GetWorldRotation(entity.mEntityID); // Push update to C++ side
      }
      set
      {
        InternalCalls.SetWorldRotation(entity.mEntityID, ref value); // Push update to C++ side
      }
    }


  public Vector3 scale
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

    public Vector3 right
    {
      get
      {
        return Vector3.Transform(new Vector3(1, 0, 0), InternalCalls.GetWorldRotation(entity.mEntityID)); // Push update to C++ side
      }
    }

    public Vector3 forward
    {
      get
      {
        return Vector3.Transform(new Vector3(0, 0, -1), InternalCalls.GetWorldRotation(entity.mEntityID)); // Push update to C++ side
    }

    }



  public Vector3 TransformDirection(Vector3 direction)
    {
        Quaternion rot = InternalCalls.GetRotation(entity.mEntityID);
        // Convert the direction to world space using the quaternion rotation
        float x = rotation.X * direction.X + rotation.W * direction.Z - rotation.Y * direction.Y;
        float y = rotation.Y * direction.X + rotation.W * direction.Y + rotation.Z * direction.Z;
        float z = rotation.Z * direction.X - rotation.Y * direction.Y + rotation.W * direction.Z;

        return new Vector3(x, y, z);
    }

   public void Rotate(Vector3 Angle)
    {
      // Convert angles from degrees to radians
      float xRad = Angle.X * Mathf.Deg2Rad;
      float yRad = Angle.Y * Mathf.Deg2Rad;
      float zRad = Angle.Z * Mathf.Deg2Rad;

      // Create a new rotation quaternion based on the given angles
      Quaternion deltaRotation = Mathf.EulertoQuat(new Vector3(xRad, yRad, zRad));

    // Apply the rotation
      worldRotation = rotation * deltaRotation; // Combine the current rotation with the new rotation
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




using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

using System.Drawing.Imaging;


public class Component
  {
    // Base class for all components
    public Entity entity { get; internal set; }

    public bool CompareTag(string s)
    {
      return (InternalCalls.GetTag(entity.mEntityID) == s);
    }
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

  public Vector3 worldPosition
  {
    get
    {
      return InternalCalls.GetWorldPosition(entity.mEntityID); // Push update to C++ side
    }
    set
    {
      InternalCalls.SetWorldPosition(entity.mEntityID, ref value); // Push update to C++ side
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

  public Vector3 rotationEuler
  {
    get
    {
      return InternalCalls.GetRotationEuler(entity.mEntityID); // Push update to C++ side
    }
    set
    {
      InternalCalls.SetRotationEuler(entity.mEntityID, ref value); // Push update to C++ side
    }
  }

  public Vector3 rotationWorldEuler
  {
    get
    {
      return InternalCalls.GetWorldRotationEuler(entity.mEntityID); // Push update to C++ side
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


  public Transform[] children
  {
    get
    {
      uint[] allChildren = InternalCalls.GetAllChildren(entity.mEntityID);
      Transform[] transforms = new Transform[allChildren.Length];
      for(int i=0;i<allChildren.Count();++i)
      {
        Entity temp = new Entity();
        temp.mEntityID = allChildren[i];
        transforms[i] = temp.GetComponent<Transform>();
      }

      return transforms;

    }

  }


    public Transform GetChild(int pos)
    {
     Transform child =  null;
      uint[] allChildren = InternalCalls.GetAllChildren(entity.mEntityID);
      if (pos >= allChildren.Length)
        Debug.LogError("You r trying to get a child transform at an invalid pos");
      else
        {
          Entity temp = new Entity();
          temp.mEntityID = allChildren[pos];
         child = temp.GetComponent<Transform>();
        }


    return child;

  }


  public Transform Find(string s)
    {
        uint childID = uint.MaxValue;
        childID = InternalCalls.FindChildByTag(entity.mEntityID, s); 
        if (childID != uint.MaxValue)
        {
          Entity child = new Entity();
          child.mEntityID = childID;
          return (child.GetComponent<Transform>());
        }
        
        Debug.LogError("Unable to find child: " + s + " inside parent: " + InternalCalls.GetTag(entity.mEntityID));
        return null;

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

  public class Text : Component
  {
    public string text
    {
      get
      {
        return InternalCalls.GetText(entity.mEntityID); // Push update to C++ side
      }
    }
  }

public class Material : Component
  {

  }


  public class Renderer : Component
  {

  }

  public class Collider : Component
  {

  }


public class Image : Component
{
  public Color color
  {
    get
    {
      Vector4 v = InternalCalls.GetImageColor(entity.mEntityID);
      return new Color(v.W, v.X, v.Y, v.Z);
    }
    set
    {
      InternalCalls.SetImageColor(entity.mEntityID, new Vector4(value.r, value.g, value.b, value.a));
    }
  }
}

public class Animation : Component
{
  public string currentAnimation
  {
    get
    {
      return InternalCalls.GetCurrentAnimation(entity.mEntityID);
    }
  }

  public bool IsPlaying()
  {
    return InternalCalls.IsPlayingAnimation(entity.mEntityID);
  }

  public void Play(string name, bool loop = false)
  {
    InternalCalls.PlayAnimation(entity.mEntityID, name, loop);
  }

  public void Pause()
  {
    InternalCalls.PauseAnimation(entity.mEntityID);
  }

  public void Resume()
  {
    InternalCalls.ResumeAnimation(entity.mEntityID);
  }

  public void StopAnimationLoop()
  {
    InternalCalls.StopAnimationLoop(entity.mEntityID);
  }
}

public class Video : Component
{
  public uint alpha
  {
    get
    {
      return InternalCalls.GetVideoAlpha(entity.mEntityID);
    }

    set
    {
      InternalCalls.SetVideoAlpha(entity.mEntityID, value);
    }
  }

  public bool audioEnabled
  {
    get
    {
      return InternalCalls.GetVideoAudioEnabled(entity.mEntityID);
    }

    set
    {
      InternalCalls.SetVideoAudioEnabled(entity.mEntityID, value);
    }
  }

  public bool started
  {
    get
    {
      return InternalCalls.HasVideoStarted(entity.mEntityID);
    }
  }

  public bool loop
  {
    get
    {
      return InternalCalls.GetVideoLoop(entity.mEntityID);
    }

    set
    {
      InternalCalls.SetVideoLoop(entity.mEntityID, value);
    }
  }

  public void Play()
  {
    InternalCalls.PlayVideo(entity.mEntityID);
  }

  // note: only works for video
  public void TogglePause()
  {
    InternalCalls.ToggleVideoPause(entity.mEntityID);
  }

  public bool HasEnded()
  {
    return InternalCalls.HasVideoEnded(entity.mEntityID);
  }

  // fill the texture with black
  public void ClearFrame()
  {
    InternalCalls.ClearVideoFrame(entity.mEntityID);
  }
}

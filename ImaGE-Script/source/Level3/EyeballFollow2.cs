using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class EyeBallFollowManager2 : Entity
{
  public Entity player;
  public float RotationSpeed { get; set; } = 5f;  // Speed in radians per second
  public Vector3 headOffset = new Vector3(0, 1.0f, 0);
  private static readonly Vector3 LocalForward = -Vector3.UnitZ;
  public bool isFollowing = false;

  EyeBallFollowManager2() : base()
  {
  }
  bool ApproximatelyEqual(Quaternion q1, Quaternion q2, float tolerance = 0.0001f)
  {
    // Dot product of two unit quaternions will be 1 if they are identical.
    return Mathf.Abs(Quaternion.Dot(q1, q2)) > 1.0f - tolerance;
  }
  // Start is called before the first frame update
  void Start()
  {
  }

  // Update is called once per frame
  void Update()
  {
    if (Global.shouldFollow)
    {
      // Calculate the player's head position using the offset.
      Vector3 headPosition = player.GetComponent<Transform>().worldPosition + headOffset;
      // Calculate the distance between the eyeball and the player's head.
      float distance = Vector3.Distance(GetComponent<Transform>().worldPosition, headPosition);

      // Calculate the direction from the eyeball to the player's head.
      Vector3 directionToHead = Vector3.Normalize(headPosition - GetComponent<Transform>().worldPosition);

      // Transform the local forward vector to world space using the eyeball's current rotation.
      Vector3 worldForward = Vector3.Transform(LocalForward, GetComponent<Transform>().rotation);

      // Calculate the rotation needed to align the world forward vector with the direction to the player's head.
      Quaternion targetRotation = Mathf.QuaternionFromToRot(worldForward, directionToHead);

      // Get the current rotation once.
      Quaternion currentRotation = GetComponent<Transform>().rotation;

      // Calculate the rotation needed to align the current forward with the direction to the player's head.
      Quaternion deltaRotation = Mathf.QuaternionFromToRot(worldForward, directionToHead);

      // Compute the final desired rotation by applying the delta to the current rotation.
      Quaternion finalTargetRotation = deltaRotation * currentRotation;

      // Now compare the current rotation with the final target rotation.
      if (ApproximatelyEqual(currentRotation, finalTargetRotation))
      {
        isFollowing = false;
      }
      else
      {
        isFollowing = true;
      }

      // Smoothly interpolate the eyeball's rotation toward the target rotation.
      GetComponent<Transform>().rotation = Quaternion.Slerp(
          GetComponent<Transform>().rotation,
          targetRotation * GetComponent<Transform>().rotation,
          RotationSpeed * Time.deltaTime
      );


      if (isFollowing)
      {
        InternalCalls.SetSoundVolume(mEntityID, "EyeballMove2_SFX.wav", 1f);
      }
      else
      {
        InternalCalls.SetSoundVolume(mEntityID, "EyeballMove2_SFX.wav", 0.0f);
      }
    }
  }
}

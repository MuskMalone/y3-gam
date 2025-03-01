/******************************************************************************/
/*!
\par        Image Engine
\file       .cs

\author     
\date       

\brief      


Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace IGE.Utils
{
  public static class Mathf
  {
    public const float PI = 3.14159265359f;
    public const float Deg2Rad = PI / 180f;
    public const float Rad2Deg = 180f / PI;
    private static readonly Random random = new Random();

    // Clamps a value between a minimum and maximum.
    public static float Clamp01(float value)
    {
      if (value < 0f)
        return 0f;
      if (value > 1f)
        return 1f;
      return value;
    }

    public static float Clamp(float value, float min, float max)
    {
      if (value < min) return min;
      if (value > max) return max;
      return value;
    }

    // Linearly interpolates between two values.
    public static float Lerp(float a, float b, float t)
    {
      return a + (b - a) * Clamp(t, 0, 1);
    }

    // Wraps a value between min and max (useful for angles)
    public static float Repeat(float value, float min, float max)
    {
      return min + (value - min) % (max - min);
    }

    // Calculates the shortest angle difference between two angles in degrees.
    public static float DeltaAngle(float current, float target)
    {
      float difference = target - current;
      return ((difference + 180f) % 360f + 360f) % 360f - 180f;
    }

    // Converts degrees to radians.
    public static float DegToRad(float degrees)
    {
      return degrees * Deg2Rad;
    }

    // Converts radians to degrees.
    public static float RadToDeg(float radians)
    {
      return radians * Rad2Deg;
    }

    public static float Sqrt(float value)
    {
      return (float)(Math.Sqrt(value));
    }

    // Eases a value to 0 or 1 based on threshold, for smoothing.
    public static float SmoothStep(float a, float b, float t)
    {
      t = Clamp((t - a) / (b - a), 0f, 1f);
      return t * t * (3f - 2f * t);
    }

    public static float CopySign(float magnitude, float sign)
    {
      // If sign is negative, set the result to be negative, otherwise positive
      return (sign < 0 ? -Abs(magnitude) : Abs(magnitude));
    }

    // Custom Cos function, takes angle in degrees
    public static float Cos(float degrees)
    {
      // Convert degrees to radians for System.Math.Cos
      double radians = degrees * Deg2Rad;
      return (float)System.Math.Cos(radians);
    }

    // Custom Sin function, takes angle in degrees
    public static float Sin(float degrees)
    {
      // Convert degrees to radians for System.Math.Sin
      double radians = degrees * Deg2Rad;
      return (float)System.Math.Sin(radians);
    }

    public static float Acos(float value)
    {
      return (float)Math.Acos(value);
    }

    public static float Abs(float value)
    {
      return value < 0 ? -value : value;
    }

    public static float Atan2(float y, float x)
    {
      if (x > 0)
        return (float)Math.Atan(y / x); // Quadrant I
      else if (x < 0 && y >= 0)
        return (float)Math.Atan(y / x) + (float)Math.PI; // Quadrant II
      else if (x < 0 && y < 0)
        return (float)Math.Atan(y / x) - (float)Math.PI; // Quadrant III
      else if (x == 0 && y > 0)
        return (float)Math.PI / 2; // Positive Y-axis
      else if (x == 0 && y < 0)
        return -(float)Math.PI / 2; // Negative Y-axis
      else
        return 0; // Undefined (0,0), you can choose how to handle this case
    }

    public static float AngleDifference(float angle1, float angle2)
    {
      // Compute the raw difference
      float difference = angle2 - angle1;

      // Wrap the difference to the range [-180, 180)
      difference = (difference + 180) % 360 - 180;

      // Ensure the result is in the range [-180, 180]
      return (difference < -180) ? difference + 360 : difference;
    }
    public static float Asin(float value)
    {
      if (value < -1 || value > 1)
        throw new ArgumentOutOfRangeException(nameof(value), "Value must be between -1 and 1.");

      return (float)Math.Asin(value);
    }

    public static double CopySign(double magnitude, double sign)
    {
      return Math.Abs(magnitude) * (sign >= 0 ? 1 : -1);
    }

    #region Quaternion


    public static Vector3 QuaternionToEuler(Quaternion q)
    {
      Vector3 angles = new Vector3();

      // roll / x
      double sinr_cosp = 2 * (q.W * q.X + q.Y * q.Z);
      double cosr_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
      angles.X = (float)Math.Atan2(sinr_cosp, cosr_cosp);

      // pitch / y
      double sinp = 2 * (q.W * q.Y - q.Z * q.X);
      if (Math.Abs(sinp) >= 1)
      {
        angles.Y = (float)Mathf.CopySign(Math.PI / 2, sinp);
      }
      else
      {
        angles.Y = (float)Math.Asin(sinp);
      }

      // yaw / z
      double siny_cosp = 2 * (q.W * q.Z + q.X * q.Y);
      double cosy_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
      angles.Z = (float)Math.Atan2(siny_cosp, cosy_cosp);

      return angles;
    }

    static Vector3 NormalizeAngles(Vector3 angles)
    {
      angles.X = NormalizeAngle(angles.X);
      angles.Y = NormalizeAngle(angles.Y);
      angles.Z = NormalizeAngle(angles.Z);
      return angles;
    }

    static float NormalizeAngle(float angle)
    {
      while (angle > 360)
        angle -= 360;
      while (angle < 0)
        angle += 360;
      return angle;
    }

    public static Quaternion EulertoQuat(Vector3 eulerDegrees)
    {
      float yaw = eulerDegrees.X * (float)(Math.PI / 180.0);
      float pitch = eulerDegrees.Y * (float)(Math.PI / 180.0);
      float roll = eulerDegrees.Z * (float)(Math.PI / 180.0);

      // Create quaternion from yaw, pitch, roll
      Quaternion newQ = Quaternion.CreateFromYawPitchRoll(yaw, pitch, roll);
      float tempX = newQ.X;
      newQ.X = newQ.Y;
      newQ.Y = tempX;
      return newQ;
     // return Quaternion.CreateFromYawPitchRoll(eulerDegrees.Y, eulerDegrees.X, eulerDegrees.Z);
    }


    public static float QuaternionAngle(Quaternion q1, Quaternion q2)
    {
      // Normalize quaternions to ensure they are unit quaternions
      q1 = Quaternion.Normalize(q1);
      q2 = Quaternion.Normalize(q2);

      // Calculate the dot product between the two quaternions
      float dot = Quaternion.Dot(q1, q2);

      // Clamp the dot product to avoid any precision issues with Mathf.Acos
      dot = Mathf.Clamp(dot, -1f, 1f);

      // Calculate the angle in radians and convert to degrees
      float angle = Mathf.Acos(dot) * 2f * Mathf.Rad2Deg;

      return angle;
    }

    #endregion

    public static int RandRange(int min, int max)
    {

      return random.Next(min, max);  // Exclusive of max
    }

    public static float RandRange(float min, float max)
    {
      return (float)(min + (max - min) * random.NextDouble());  // Inclusive of max
    }

    public static Vector3 QuatMulVec3(Quaternion rotation, Vector3 point)
    {
      // Extract quaternion components
      float x = rotation.X * 2.0f;
      float y = rotation.Y * 2.0f;
      float z = rotation.Z * 2.0f;
      float xx = rotation.X * x;
      float yy = rotation.Y * y;
      float zz = rotation.Z * z;
      float xy = rotation.X * y;
      float xz = rotation.X * z;
      float yz = rotation.Y * z;
      float wx = rotation.W * x;
      float wy = rotation.W * y;
      float wz = rotation.W * z;

      // Apply rotation to the vector
      Vector3 result;
      result.X = (1.0f - (yy + zz)) * point.X + (xy - wz) * point.Y + (xz + wy) * point.Z;
      result.Y = (xy + wz) * point.X + (1.0f - (xx + zz)) * point.Y + (yz - wx) * point.Z;
      result.Z = (xz - wy) * point.X + (yz + wx) * point.Y + (1.0f - (xx + yy)) * point.Z;
      return result;
    }

  }
}
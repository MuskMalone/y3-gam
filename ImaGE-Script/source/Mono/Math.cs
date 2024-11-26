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
      return (difference + 180f) % 360f - 180f;
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

    public static float Asin(float value)
    {
      if (value < -1 || value > 1)
        throw new ArgumentOutOfRangeException(nameof(value), "Value must be between -1 and 1.");

      return (float)Math.Asin(value);
    }

    #region Quaternion

    public static Vector3 QuatToEuler(Quaternion q)
    {
      Vector3 euler;

      // Pitch (X-axis rotation)
      float sinp = 2 * (q.W * q.X - q.Z * q.Y);
      if (Mathf.Abs(sinp) >= 1)
        euler.X = Mathf.RadToDeg(sinp > 0 ? Mathf.PI / 2 : -Mathf.PI / 2); // Clamp to 90 or -90 degrees
      else
        euler.X = Mathf.RadToDeg(Mathf.Asin(sinp));

      // Yaw (Y-axis rotation)
      float siny_cosp = 2 * (q.W * q.Y + q.Z * q.X);
      float cosy_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
      euler.Y = Mathf.RadToDeg(Mathf.Atan2(siny_cosp, cosy_cosp));

      // Roll (Z-axis rotation)
      float sinr_cosp = 2 * (q.W * q.Z + q.X * q.Y);
      float cosr_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
      euler.Z = Mathf.RadToDeg(Mathf.Atan2(sinr_cosp, cosr_cosp));

      return euler;
    }


    public static Quaternion EulertoQuat(Vector3 euler)
    {
      // Convert angles from degrees to radians
      euler.X *= Mathf.Deg2Rad;
      euler.Y *= Mathf.Deg2Rad;
      euler.Z *= Mathf.Deg2Rad;

      // Calculate the half angles
      float cx = Mathf.Cos(euler.X * 0.5f);
      float sx = Mathf.Sin(euler.X * 0.5f);
      float cy = Mathf.Cos(euler.Y * 0.5f);
      float sy = Mathf.Sin(euler.Y * 0.5f);
      float cz = Mathf.Cos(euler.Z * 0.5f);
      float sz = Mathf.Sin(euler.Z * 0.5f);

      // Compute quaternion components
      float w = cx * cy * cz + sx * sy * sz;
      float xVal = sx * cy * cz - cx * sy * sz;
      float yVal = cx * sy * cz + sx * cy * sz;
      float zVal = cx * cy * sz - sx * sy * cz;

      return new Quaternion(xVal, yVal, zVal, w);
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
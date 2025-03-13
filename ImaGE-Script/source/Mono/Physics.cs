using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace IGE.Utils
{
  [StructLayout(LayoutKind.Sequential)]
  public struct ContactPoint
  {
    /// <summary>
    /// The position of the contact point between the shapes, in world space.
    /// </summary>
    public Vector3 position;

    /// <summary>
    /// The separation of the shapes at the contact point. A negative value denotes a penetration.
    /// </summary>
    public float separation;

    /// <summary>
    /// The normal of the contacting surfaces at the contact point.
    /// The normal direction points from the second shape to the first shape.
    /// </summary>
    public Vector3 normal;

    /// <summary>
    /// The surface index of shape 0 at the contact point.
    /// Used to identify the surface material.
    /// </summary>
    public uint internalFaceIndex0;

    /// <summary>
    /// The impulse applied at the contact point, in world space.
    /// Divide by the simulation time step to get a force value.
    /// </summary>
    public Vector3 impulse;

    /// <summary>
    /// The surface index of shape 1 at the contact point.
    /// Used to identify the surface material.
    /// </summary>
    public uint internalFaceIndex1;
  }
}

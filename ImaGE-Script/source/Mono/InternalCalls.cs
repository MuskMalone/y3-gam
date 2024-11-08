/******************************************************************************/
/*!
\file     InternalCalls.cs

\author   Han Qin Ding(han.q@digipen.edu), Ernest Cheo (e.cheo@digipen.edu)
\date     28 Septmeber 2024

\brief      
  All C# internal calls go here, where information from CPP code 
  can be accessed in C#, and vice versa

Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents with the prior written 
consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using System.Numerics;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Image.Utils
{
  public static class InternalCalls
  {
    #region Transform

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetScale(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetScale(uint entityHandle, ref Vector3 scale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetWorldPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPosition(uint entityHandle, ref Vector3 position);

    // avoid using this, prefer SetPosition (local)
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldPosition(uint entityHandle, ref Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Quaternion GetRotation(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetRotation(uint entityHandle, ref Quaternion position);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Quaternion GetWorldRotation(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldRotation(uint entityHandle, ref Quaternion position);

    //[MethodImplAttribute(MethodImplOptions.InternalCall)]
    //internal extern static void GetRotation(ref uint entityHandle, ref float rotation);

    //[MethodImplAttribute(MethodImplOptions.InternalCall)]
    //internal extern static void SetRotation(ref uint entityHandle, ref float rotation);

    #endregion


    #region Input
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyTriggered(KeyCode key);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyHeld(KeyCode key);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetAxis(string s);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool anyKeyDown();


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetMouseDelta();
    #endregion


    #region Logging
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void Log(string s);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void LogWarning(string s);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void LogError(string s);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void LogCritical(string s);
    #endregion


    #region Entity

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static string GetTag(uint EntityID);


    // avoid using this, prefer SetPosition (local)
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetActive(uint entityHandle, bool active);


    #endregion


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetDeltaTime();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void MoveCharacter(uint mEntityID, Vector3 dVec);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsGrounded(uint mEntityID);


  }
}

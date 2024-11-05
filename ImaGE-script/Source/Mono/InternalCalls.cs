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
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Image.Mono.Utils
{
  public static class InternalCalls
  {
    #region Transform

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vec3<float> GetScale(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetScale(uint entityHandle, ref Vec3<float> scale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vec3<float> GetWorldPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vec3<float> GetPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPosition(uint entityHandle, ref Vec3<float> position);

    // avoid using this, prefer SetPosition (local)
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldPosition(uint entityHandle, ref Vec3<float> position);

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

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string GetTag(uint EntityID);
    
  }
}

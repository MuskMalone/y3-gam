/******************************************************************************/
/*!
\par        Image Engine
\file       InternalCalls.cs

\author     Han Qin Ding(han.q@digipen.edu), Ernest Cheo (e.cheo@digipen.edu)
\date       Sep 28, 2024

\brief      All C# internal calls go here, where information from CPP code 
            can be accessed in C#, and vice versa

\copyright  Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
            or disclosure of this file or its contents withthe prior
            written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Image.Mono
{
  public static class InternalCalls
  {
    #region Transform

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vec3<float> GetWorldScale(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldScale(ref uint entityHandle, ref Vec3<float> scale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vec3<float> GetWorldPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vec3<float> GetPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPosition(uint entityHandle, ref Vec3<float> position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldPosition(uint entityHandle, ref Vec3<float> position);

    //[MethodImplAttribute(MethodImplOptions.InternalCall)]
    //internal extern static void GetRotation(ref uint entityHandle, ref float rotation);

    //[MethodImplAttribute(MethodImplOptions.InternalCall)]
    //internal extern static void SetRotation(ref uint entityHandle, ref float rotation);

    #endregion

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyTriggered(KeyCode key);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyHeld(KeyCode key);
  }
}

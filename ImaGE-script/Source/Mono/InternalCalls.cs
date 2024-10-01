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

namespace Image
{
  public static class InternalCalls
  {
    #region Transform
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void GetTranslation(ref uint entityHandle, ref Vec3<double> translation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetTranslation(ref uint entityHandle, ref Vec3<double> translation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void GetRotation(ref uint entityHandle, ref float rotation);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetRotation(ref uint entityHandle, ref float rotation);

    #endregion
  }
}

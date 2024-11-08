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
  public static class Debug
  {
    public static void Log(string s)
    {
      InternalCalls.Log(s);
    }

    public static void LogWarning(string s)
    {
      InternalCalls.LogWarning(s);
    }


    public static void LogError(string s)
    {
      InternalCalls.LogError(s);
    }


    public static void LogCritical(string s)
    {
      InternalCalls.LogCritical(s);
    }




  }
}
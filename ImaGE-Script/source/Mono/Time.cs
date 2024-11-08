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
  public static class Time
  {
    public static float deltaTime
    {
      get
      {
        return InternalCalls.GetDeltaTime(); // Push update to C++ side
      }
    }

  }
}
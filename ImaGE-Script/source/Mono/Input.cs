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


using Image.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace Image.Utils
{
  public static class Input
  {
    public static bool GetMouseButtonDown(int button)
    {
      if ((KeyCode)button > KeyCode.MOUSE_BUTTON_8 || (KeyCode)button < KeyCode.MOUSE_BUTTON_1)
        return false;
      return InternalCalls.IsKeyHeld((KeyCode) button) || InternalCalls.IsKeyTriggered((KeyCode)button);
    }

    public static bool GetKeyDown(KeyCode button)
    {
      return InternalCalls.IsKeyHeld(button) || InternalCalls.IsKeyTriggered(button);
    }

    public static float GetAxis(string s)
    {
      return InternalCalls.GetAxis(s);
    }

      public static bool anyKeyDown
      {
        get
        {
          return InternalCalls.anyKeyDown(); // Push update to C++ side
        }
      }


  }
}
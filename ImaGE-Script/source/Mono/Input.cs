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

    public static Vector3 mousePosition // Checks if any key is triggered/held this frame and return the result
    {
      get
      {
        return InternalCalls.GetMousePos();
      }
    }

    public static bool anyKeyDown // Checks if any key is triggered/held this frame and return the result
    {
      get
      {
        return InternalCalls.AnyKeyDown(); 
      }
    }

    public static string inputString // Returns a string containing all the keys triggered/held this frame
    {
      get
      {
        return InternalCalls.GetInputString(); 
      }
    }



  }
}
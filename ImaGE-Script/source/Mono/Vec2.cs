/*!*********************************************************************
\file   Vec2.cs
\author han.q\@digipen.edu
\date   28-September-2023
\brief
	This file contains a simple vec2 template struct.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Image.Utils
{
   public struct Vec2<T>
  {
    public T X { get; set; }
    public T Y { get; set; }

    /*!*********************************************************************
    \brief
      Non default constructor of Vec template struct

    \params xVal
      value for the vec2<T>.x

    \params yVal
      value for the vec2<T>.y
    ************************************************************************/
    public Vec2(T xVal, T yVal)
    {
      X = xVal;
      Y = yVal;
    }

    public Vec2(Vec3<T> rhs)
    {
      X = rhs.X;
      Y = rhs.Y;
    }
  }
}

/*!*********************************************************************
\file   Vec4.cs
\author Han Qin Ding
\date   4-October-2024
\brief
	This file contains a simple vec4 template struct.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IGE.Utils
{ 
  public struct Vec4<T>
  {    
    public T X { get; set; }
    public T Y { get; set; }
    public T Z { get; set; }
    public T W { get; set; }

    /*!*********************************************************************
    \brief
      Non default constructor of Vec template struct

    \params xVal
      value for the vec4<T>.x
    \params yVal
      value for the vec4<T>.y
    \params zVal
      value for the vec4<T>.z
    \params wVal
      value for the vec4<T>.w
    ************************************************************************/
    public Vec4(T xVal, T yVal, T zVal, T wVal)
    {
      X = xVal;
      Y = yVal;
      Z = zVal;
      W = wVal;
    }

    /*!*********************************************************************
    \brief
      Non default constructor of Vec template struct

    \params xy
      value for the vec2<T>.xy
    \params zVal
      value for the vec4<T>.z
    \params wVal
      value for the vec4<T>.w
    ************************************************************************/
    public Vec4(Vec2<T> xy, T zVal, T wVal)
    {
      X = xy.X;
      Y = xy.Y;
      Z = zVal;
      W = wVal;
    }

    /*!*********************************************************************
    \brief
      Non default constructor of Vec template struct

    \params xyz
      value for the vec2<T>.xyz
    \params wVal
      value for the vec4<T>.w
    ************************************************************************/
    public Vec4(Vec3<T> xyz, T wVal)
    {
      X = xyz.X;
      Y = xyz.Y;
      Z = xyz.Z;
      W = wVal;
    }
  }
}

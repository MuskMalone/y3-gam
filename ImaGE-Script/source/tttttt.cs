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
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class tttttt : Entity
{
  // Start is called before the first frame update
  void Start()
  {
    GetComponent<Video>().ClearFrame();
  }

  // Update is called once per frame
  void Update()
  {
    Video vid = GetComponent<Video>();
    if (Input.GetKeyTriggered(KeyCode.RIGHT_BRACKET))
    {
      vid.Play();
    }

    if (Input.GetKeyTriggered(KeyCode.SEMICOLON))
    {
      vid.TogglePause();
    }
  }
}



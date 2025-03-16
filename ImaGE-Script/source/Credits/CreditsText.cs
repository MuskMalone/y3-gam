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
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class CreditsText : Entity
{
  public Entity Credits;
  public Entity Designer;
  public Entity DesignerName;
  public Entity Programmers;
  public Entity ProgrammersName;
  public float speed;

  CreditsText() : base()
  {

  }
  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    Credits.GetComponent<Transform>().position = Credits.GetComponent<Transform>().position + new Vector3(0, speed, 0) * Time.deltaTime;
    Designer.GetComponent<Transform>().position = Designer.GetComponent<Transform>().position + new Vector3(0, speed, 0) * Time.deltaTime;
    DesignerName.GetComponent<Transform>().position = DesignerName.GetComponent<Transform>().position + new Vector3(0, speed, 0) * Time.deltaTime;
    Programmers.GetComponent<Transform>().position = Programmers.GetComponent<Transform>().position + new Vector3(0, speed, 0) * Time.deltaTime;
    ProgrammersName.GetComponent<Transform>().position = ProgrammersName.GetComponent<Transform>().position + new Vector3(0, speed, 0) * Time.deltaTime;

    if(Input.GetKeyTriggered(KeyCode.ESCAPE))
    {
      InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\mainmenu.scn");
    }
  }
}



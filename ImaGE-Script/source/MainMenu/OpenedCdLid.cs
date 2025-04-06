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
using System.Numerics;

public class OpenedCdLid : Entity
{
  public NewGameCD newGameCD;
  public NewGameCD creditsCD;
  public NewGameCD settingsCD;
  public Entity openLid, closedLid;
  public bool isCDInPlayer = false;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    if (InternalCalls.OnTriggerEnter(mEntityID, newGameCD.mEntityID)
        || InternalCalls.OnTriggerEnter(mEntityID, creditsCD.mEntityID)
        || InternalCalls.OnTriggerEnter(mEntityID, settingsCD.mEntityID))
    {
      openLid.SetActive(true);
      closedLid.SetActive(false);
      isCDInPlayer = true;
    }
    else if (InternalCalls.OnTriggerExit(mEntityID, newGameCD.mEntityID)
        || InternalCalls.OnTriggerExit(mEntityID, creditsCD.mEntityID)
        || InternalCalls.OnTriggerExit(mEntityID, settingsCD.mEntityID))
    {
      closedLid.SetActive(true);
      openLid.SetActive(false);
      isCDInPlayer = false;
    }
  }
}


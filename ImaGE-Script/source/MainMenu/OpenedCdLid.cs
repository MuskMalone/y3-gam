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
    //private CDDragAndDrop cDDragAndDrop;
    public Vector3 outOfTheWay = new Vector3(10.0f, 10.0f, 10.0f);

    private Vector3 originalPosition;
    //private Quaternion originalRotation;
    private Vector3 openCDpos = new Vector3(0.382f, -0.308f, -0.442f);
    private NewGameCD newGameCD;
    // Start is called before the first frame update
    void Start()
    {
        newGameCD = FindObjectOfType<NewGameCD>();
        originalPosition = InternalCalls.GetWorldPosition(mEntityID);
        //originalRotation = InternalCalls.GetWorldRotation(mEntityID);

        string tag = InternalCalls.GetTag(mEntityID);
        if (tag == "OpenCircular_Lid")
        {
            InternalCalls.SetWorldPosition(mEntityID, ref outOfTheWay);
        }
        //else if (tag == "ClosedCircular_Lid")
        //{

        //}
    }

    // Update is called once per frame
    void Update()
    {
        string tag = InternalCalls.GetTag(mEntityID);
        if (newGameCD.isLidOpen)
        {
            if (tag == "OpenCircular_Lid")
            {
                InternalCalls.SetWorldPosition(mEntityID, ref openCDpos);
            }
            else if (tag == "ClosedCircular_Lid")
            {
                InternalCalls.SetWorldPosition(mEntityID, ref outOfTheWay);

      }
    }
        else
        {
            if (tag == "OpenCircular_Lid")
            {
                InternalCalls.SetWorldPosition(mEntityID, ref outOfTheWay);
      }
            else if (tag == "ClosedCircular_Lid")
            {
                InternalCalls.SetWorldPosition(mEntityID, ref originalPosition);
      }
        }
    }
}



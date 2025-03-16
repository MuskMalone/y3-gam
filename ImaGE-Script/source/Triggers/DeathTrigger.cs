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
using IGE.Utils;

public class DeathTrigger : Entity
{
    public PlayerMove playerMove;
    private Vector3 respawnPosition = new Vector3(-43, -0.565f, -64.572f);
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
        {
            //Console.WriteLine("death triggered");
            InternalCalls.SetPosition(playerMove.mEntityID, ref respawnPosition);
        }
    }


}



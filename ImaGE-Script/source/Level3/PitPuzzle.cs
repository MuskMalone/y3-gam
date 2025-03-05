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
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;

public class PitPuzzle : Entity
{
    // Start is called before the first frame update
    public Entity VisiblePlanks;
    public Entity InvisiblePlanks;

    void Start()
    {
        VisiblePlanks.SetActive(false);
        InvisiblePlanks.SetActive(true);
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyTriggered(KeyCode.EQUAL))
        {
            VisiblePlanks.SetActive(true);
            InvisiblePlanks.SetActive(false);
        }
    }
}



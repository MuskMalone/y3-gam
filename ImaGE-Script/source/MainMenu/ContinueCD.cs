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

public class ContinueCD : Entity
{
    public bool isLidOpen;
    private bool hasOpenSoundPlayed;
    private bool hasCloseSoundPlayed;
    // Start is called before the first frame update
    void Start()
    {
        isLidOpen = false;
        hasOpenSoundPlayed = false;
        hasCloseSoundPlayed = false;
    }

    // Update is called once per frame
    void Update()
    {

    }

    public void OnMouseDown()
    {

        isLidOpen = true;
        hasCloseSoundPlayed = false;
        if (!hasOpenSoundPlayed)
        {
            InternalCalls.PlaySound(mEntityID, "OpenConsole_SFX");
            hasOpenSoundPlayed = true;
        }
    }

    public void OnMouseUp()
    {

        hasOpenSoundPlayed = false;
        bool mouseOnCDPlayer = false;
        Vector3 MousePos = InternalCalls.GetMousePosWorld(1.0f);
        Vector3 MousPosRayEnd = MousePos + (InternalCalls.GetCameraForward() * 5.0f);
        uint hitEntity = InternalCalls.Raycast(MousePos, MousPosRayEnd);

        if (InternalCalls.GetTag(hitEntity) == "CDPlayer_Body")
        {

            mouseOnCDPlayer = true;
        }

        if (mouseOnCDPlayer)
        {
            isLidOpen = true;
            InternalCalls.PlaySound(mEntityID, "PlaceCD_SFX");
        }
        else
        {
            isLidOpen = false;
            if (!hasCloseSoundPlayed)
            {
                InternalCalls.PlaySound(mEntityID, "CloseConsole_SFX");
                hasCloseSoundPlayed = true;
            }
        }
    }
}




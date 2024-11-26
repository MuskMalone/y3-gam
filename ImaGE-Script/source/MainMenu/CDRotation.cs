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

public class CDRotation : Entity
{
    public float rotationSpeed = 100f;
    private bool isHovered;
    private Quaternion originalRotation;
    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Start");
        originalRotation = InternalCalls.GetRotation(mEntityID);
    }

    // Update is called once per frame
    void Update()
    {
        if(isHovered)
        {
            RotateEntity(10f); 
        }
    }

    private void OnMouseEnter()
    {
        Debug.Log("MouseEntered");
        isHovered = true;
    }

    private void OnMouseExit()
    {
        isHovered = false;
        InternalCalls.SetRotation(mEntityID, ref originalRotation);
    }

    void RotateEntity(float rotationSpeed)
    {
        //Debug.Log("did it get in here");
        // Get the current rotation as a Quaternion
        Quaternion currentRotation = InternalCalls.GetRotation(mEntityID);

        // Create the desired rotation based on Vector3.up and rotationSpeed
        Quaternion rotationChange = Quaternion.CreateFromAxisAngle(Vector3.UnitY, Mathf.DegToRad(rotationSpeed * Time.deltaTime));

        // Combine the current rotation with the new rotation
        Quaternion newRotation = Quaternion.Multiply(currentRotation, rotationChange);

        // Apply the new rotation
        InternalCalls.SetRotation(mEntityID, ref newRotation);
    }
}



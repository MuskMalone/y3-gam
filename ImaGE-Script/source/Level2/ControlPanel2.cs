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

public class ControlPanel2 : Entity
{
    public PlayerInteraction playerInteraction;
    public Entity UVLight;
    public Entity[] hiddenText0;
    public Entity[] hiddenText1;
    //public Entity[][] hiddenTexts; // Stores all hidden texts for each mode
    public int activeModeIndex = 0;
    public Entity playerCamera;
    public Entity controlPanelCamera;
    public PlayerMove playerMove;
    public float rotationSpeed = 50.0f; // degrees per second

    public float minVerticalRotation = -17.0f; // Minimum pitch
    public float maxVerticalRotation = 25.0f;  // Maximum pitch
    public float minHorizontalRotation = -36.0f; // Minimum yaw
    public float maxHorizontalRotation = 32.0f;  // Maximum yaw

    private bool controllingLights = false;
    public int currMode = 1;
    //public LightSwitch lightSwitch;
    public Entity[] LightsToToggleActive;
    public ControlPanel2() : base()
    {

    }

    void Start()
    {
        if (playerMove == null)
        {
            Debug.LogError("[ControlPanel2.cs] PlayerMove Script Entity not found!");
            return;
        }

        foreach (Entity text in hiddenText0)
        {
            text?.SetActive(false);
        }

        foreach (Entity text in hiddenText1)
        {
            text?.SetActive(false);
        }
    }

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        bool isPanelHit = playerInteraction.RayHitString == "ControlPanel2";
        
        if(!controllingLights)
        {
            if(Input.GetKeyDown(KeyCode.N))
            {
                currMode = 1;
                //SwitchMode(0);
            }
            else if(Input.GetKeyDown(KeyCode.M))
            {
                currMode = 2;
                //SwitchMode(1);
            }
        }

        if (mouseClicked && isPanelHit)
        {
            SetControlPanelCameraAsMain();
            controllingLights = true;

            if (AreAllLightsOff())
            {
                if (currMode == 1)
                {
                    foreach (Entity text in hiddenText0)
                    {
                        text?.SetActive(true);
                    }
                }
                else if(currMode == 2) 
                {
                    foreach (Entity text in hiddenText1)
                    {
                        text?.SetActive(true);
                    }
                }
            }
        }

        if (controllingLights)
        {
            playerMove.FreezePlayer();
            Vector3 currentRotation = InternalCalls.GetRotationEuler(UVLight.mEntityID);
            float deltaTime = Time.deltaTime;

            if (Input.GetKeyDown(KeyCode.W))
            {
                //Console.WriteLine("Entered W");
                currentRotation.X += rotationSpeed * deltaTime;
            }

            if (Input.GetKeyDown(KeyCode.S))
            {
                //Console.WriteLine("Entered S");
                currentRotation.X -= rotationSpeed * deltaTime;
            }

            if (Input.GetKeyDown(KeyCode.A))
            {
                currentRotation.Y += rotationSpeed * deltaTime;
            }

            if (Input.GetKeyDown(KeyCode.D))
            {
                currentRotation.Y -= rotationSpeed * deltaTime;
            }

            currentRotation.X = Mathf.Clamp(currentRotation.X, minVerticalRotation, maxVerticalRotation);
            currentRotation.Y = Mathf.Clamp(currentRotation.Y, minHorizontalRotation, maxHorizontalRotation);
            InternalCalls.SetRotationEuler(UVLight.mEntityID, ref currentRotation);

            if (Input.GetKeyTriggered(KeyCode.ESCAPE))
            {
                controllingLights = false;
                playerMove.UnfreezePlayer();
                if (currMode == 1)
                {
                    foreach (Entity text in hiddenText0)
                    {
                        text?.SetActive(false);
                    }
                }
                else if (currMode == 2)
                {
                    foreach (Entity text in hiddenText1)
                    {
                        text?.SetActive(false);
                    }
                }
                SetPlayerCameraAsMain();
            }
        }

        UVLight.SetActive(controllingLights);
    }

    private void SetPlayerCameraAsMain()
    {
        InternalCalls.SetTag(playerCamera.mEntityID, "MainCamera");
        InternalCalls.SetTag(controlPanelCamera.mEntityID, "PanelCamera");
    }

    private void SetControlPanelCameraAsMain()
    {
        InternalCalls.SetTag(playerCamera.mEntityID, "PlayerCamera");
        InternalCalls.SetTag(controlPanelCamera.mEntityID, "MainCamera");
    }

    private bool AreAllLightsOff()
    {
        foreach (Entity light in LightsToToggleActive)
        {
            if (light.IsActive())
            {
                return false;
            }
        }
        return true;
    }

    //public void SwitchMode(int index)
    //{
        
    //}
}





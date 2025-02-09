﻿/******************************************************************************/
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
    public Entity[] hiddenText2;
    public Entity[] hiddenText3;
    public Entity zeusStatue;
    public Entity poseidonStatue;
    public Entity artemisStatue;
    public Entity dionysusStatue;

    public bool zeusStatueVisible = false;
    public bool poseidonStatueVisible = false;
    public bool artemisStatueVisible = false;
    public bool dionysusStatueVisible = false;


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

    private bool isUVLightActive = false;
    private bool areNumbersActive = false;
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

        foreach (Entity text in hiddenText2)
        {
            text?.SetActive(false);
        }

        foreach (Entity text in hiddenText3)
        {
            text?.SetActive(false);
        }

        zeusStatue.SetActive(true);
        poseidonStatue.SetActive(false);
        artemisStatue.SetActive(false);
        dionysusStatue.SetActive(false);

        UVLight.SetActive(false);
    }

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        bool isPanelHit = playerInteraction.RayHitString == "ControlPanel2";
        
        //when painting used
        if(!controllingLights)
        {
            //if(Input.GetKeyDown(KeyCode.N))
            if(!dionysusStatueVisible && zeusStatueVisible && !poseidonStatueVisible && !artemisStatueVisible)
            {
                if (AreAllLightsOff() && isUVLightActive)
                {
                    DisableMode(currMode);
                }
                currMode = 1;
                DisableAllStatues();
                zeusStatue.SetActive(true);
                if(AreAllLightsOff() && isUVLightActive)
                {
                    SwitchMode(currMode);
                }
                
                //SwitchMode(0);
            }
            //else if(Input.GetKeyDown(KeyCode.M))
            else if (!dionysusStatueVisible && !zeusStatueVisible && poseidonStatueVisible && !artemisStatueVisible)
            {
                if (AreAllLightsOff() && isUVLightActive)
                {
                    DisableMode(currMode);
                }
                currMode = 2;
                DisableAllStatues();
                poseidonStatue.SetActive(true);
                if (AreAllLightsOff() && isUVLightActive)
                {
                    SwitchMode(currMode);
                }
                //SwitchMode(1);
            }
            //else if (Input.GetKeyDown(KeyCode.Y))
            else if (!dionysusStatueVisible && !zeusStatueVisible && !poseidonStatueVisible && artemisStatueVisible)
            {
                
                if (AreAllLightsOff() && isUVLightActive)
                {
                    //Console.WriteLine("currMode before" + currMode);
                    DisableMode(currMode);
                }
                currMode = 3;
                DisableAllStatues();
                artemisStatue.SetActive(true);
                //Console.WriteLine("currMode after" + currMode);
                if (AreAllLightsOff() && isUVLightActive)
                {
                    
                    SwitchMode(currMode);
                }
                //SwitchMode(1);
            }
            //else if (Input.GetKeyDown(KeyCode.U))
            else if (dionysusStatueVisible && !zeusStatueVisible && !poseidonStatueVisible && !artemisStatueVisible) 
            {
                if (AreAllLightsOff() && isUVLightActive)
                {
                    DisableMode(currMode);
                }
                currMode = 4;
                DisableAllStatues();
                dionysusStatue.SetActive(true);
                if (AreAllLightsOff() && isUVLightActive)
                {
                    SwitchMode(currMode);
                }
                //SwitchMode(1);
            }
        }

        //i can use the control panel
        if (mouseClicked && isPanelHit)
        {
            SetControlPanelCameraAsMain();
            controllingLights = true;

            //if the lights are off
            if (AreAllLightsOff())
            {
                //i turn on the currentmode number
                SwitchMode(currMode);
                //numbers are visible
                areNumbersActive = true;
            }
        }

        //if the light are on
        if(!AreAllLightsOff())
        {
            //i turn off the numbers
            DisableMode(currMode);
            //numbers are invisible
            areNumbersActive = false;
        }
        //if the lights are off and the numbers are invisible
        if(AreAllLightsOff() && !areNumbersActive)
        {
            //turn on the numbers
            SwitchMode(currMode);
            //numbers visible
            areNumbersActive = true;
        }

        
        if (controllingLights)
        {
            if(!isUVLightActive)
            {
                UVLight.SetActive(true);
                isUVLightActive = true;
            }
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
                //DisableMode(currMode);
                
                SetPlayerCameraAsMain();
            }
        }

        //UVLight.SetActive(controllingLights);
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

    public void DisableMode(int index)
    {
        if (index == 1)
        {
            foreach (Entity text in hiddenText0)
            {
                text?.SetActive(false);
            }
        }
        else if (index == 2)
        {
            foreach (Entity text in hiddenText1)
            {
                text?.SetActive(false);
            }
        }
        else if (index == 3)
        {
            foreach (Entity text in hiddenText2)
            {
                text?.SetActive(false);
            }
        }
        else if (index == 4)
        {
            foreach (Entity text in hiddenText3)
            {
                text?.SetActive(false);
            }
        }
    }

    public void DisableAllStatues()
    {
        zeusStatue.SetActive(false);
        poseidonStatue.SetActive(false);
        artemisStatue.SetActive(false);
        dionysusStatue.SetActive(false);
    }


    public void SwitchMode(int index)
    {
        if (index == 1)
        {
            foreach (Entity text in hiddenText0)
            {
                text?.SetActive(true);
            }
            
        }
        else if (index == 2)
        {
            foreach (Entity text in hiddenText1)
            {
                text?.SetActive(true);
            }
            
        }
        else if (index == 3)
        {
            foreach (Entity text in hiddenText2)
            {
                text?.SetActive(true);
            }
            
        }
        else if (index == 4)
        {
            foreach (Entity text in hiddenText3)
            {
                text?.SetActive(true);
            }
            
        }
    }
}





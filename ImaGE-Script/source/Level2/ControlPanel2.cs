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

	private Entity[][] hiddenTexts; // Stores all hidden texts for each mode
	public Entity playerCamera;
	public Entity controlPanelCamera;
	public PlayerMove playerMove;
	public float rotationSpeed = 50.0f; // degrees per second

	public float minVerticalRotation = -17.0f; // Minimum pitch
	public float maxVerticalRotation = 25.0f;  // Maximum pitch
	public float minHorizontalRotation = -36.0f; // Minimum yaw
	public float maxHorizontalRotation = 32.0f;  // Maximum yaw

	// also used for indexing hiddenTexts arr
	public enum StatueType
	{
		ZEUS = 0,
		DIONYSUS,
		POSEIDON,
		ARTEMIS
	}
	private StatueType currStatue = StatueType.ZEUS;	// current statue in place

	private enum State
	{
		CLOSED,			// door closed (pre-animation)
		OPEN,			// unlocked, interaction is allowed
		CONTROL_PANEL,	// when interacting with lights ON
		UV_LIGHT		// UV light (when lights off)
	}

	private State currState = State.CLOSED;
	private bool areLightsOn = true;

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

		// workaround since inspector cant display 2D Entity array
		InitHiddenTexts();

		zeusStatue.SetActive(true);
		poseidonStatue.SetActive(false);
		artemisStatue.SetActive(false);
		dionysusStatue.SetActive(false);

		UVLight.SetActive(false);
	}

	void Update()
	{
		switch (currState)
		{
			// after control panel animation has triggered
			case State.OPEN:
				bool mouseClicked = Input.GetMouseButtonTriggered(0);
				bool isPanelHit = playerInteraction.RayHitString == "ControlPanel";

				if (mouseClicked && isPanelHit)
				{
                    Debug.Log("HIT");
                    InternalCalls.PlaySound(mEntityID, "UVLight");
					SetControlPanelCameraAsMain();
					playerMove.FreezePlayer();

					// transition to the next state depending on whether lights are on
					if (!areLightsOn)
					{
						SetHiddenText(currStatue, true);
						UVLight.SetActive(true);
						currState = State.UV_LIGHT;
					}
					else
					{
						currState = State.CONTROL_PANEL;
					}
				}

				break;

			// no UV light: simply check for ESC input
			case State.CONTROL_PANEL:
                
                if (Input.GetKeyTriggered(KeyCode.ESCAPE))
				{
					playerMove.UnfreezePlayer();
					SetPlayerCameraAsMain();

					currState = State.OPEN; // return to OPEN state
				}
				break;

			// UV light: enable controls and check for ESC input
			case State.UV_LIGHT:
				Vector3 currentRotation = InternalCalls.GetRotationEuler(UVLight.mEntityID);
				float deltaTime = Time.deltaTime;
				bool playerInput = false;

				if (Input.GetKeyDown(KeyCode.W))
				{
					//Console.WriteLine("Entered W");
					currentRotation.X += rotationSpeed * deltaTime;
					playerInput = true;
				}
				if (Input.GetKeyDown(KeyCode.S))
				{
					//Console.WriteLine("Entered S");
					currentRotation.X -= rotationSpeed * deltaTime;
					playerInput = true;
				}
				if (Input.GetKeyDown(KeyCode.A))
				{
					currentRotation.Y += rotationSpeed * deltaTime;
					playerInput = true;
				}
				if (Input.GetKeyDown(KeyCode.D))
				{
					currentRotation.Y -= rotationSpeed * deltaTime;
					playerInput = true;
				}

				// only update if there was input
				if (playerInput)
				{
					currentRotation.X = Mathf.Clamp(currentRotation.X, minVerticalRotation, maxVerticalRotation);
					currentRotation.Y = Mathf.Clamp(currentRotation.Y, minHorizontalRotation, maxHorizontalRotation);
					InternalCalls.SetRotationEuler(UVLight.mEntityID, ref currentRotation);
				}

				if (Input.GetKeyTriggered(KeyCode.ESCAPE))
				{
					playerMove.UnfreezePlayer();
                    SetPlayerCameraAsMain();

					// hide the hiddenText
					SetHiddenText(currStatue, false);
                    UVLight.SetActive(false);
					currState = State.OPEN; // return to OPEN state
				}
				break;

			// do nothing
			case State.CLOSED:
				break;
			default:
				break;
		}
	}

	// called by PictureAlign when painting is used
	public void SwitchMode(StatueType statue)
	{
		// set the next statue as the only active
		DisableAllStatues();
		switch (statue)
		{
			case StatueType.ZEUS:
				zeusStatue.SetActive(true);
				break;
			case StatueType.POSEIDON:
				poseidonStatue.SetActive(true);
				break;
			case StatueType.ARTEMIS:
				artemisStatue.SetActive(true);
				break;
			case StatueType.DIONYSUS:
				dionysusStatue.SetActive(true);
				break;
			default:
				break;
		}

		// update the current statue
		currStatue = statue;
	}

	// called by LightSwitch when toggled
	public void LightsToggled(bool isOn)
	{
        areLightsOn = isOn;
	}

	// called by ControlPanelDoor when animation begins
	public void Unlock()
	{
		currState = State.OPEN;
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

	public void SetHiddenText(StatueType statue, bool active)
	{
		foreach (Entity text in hiddenTexts[(int)statue])
		{
			text?.SetActive(active);
		}
	}

	public void DisableAllStatues()
	{
		zeusStatue.SetActive(false);
		poseidonStatue.SetActive(false);
		artemisStatue.SetActive(false);
		dionysusStatue.SetActive(false);
	}

	private void InitHiddenTexts()
	{
		hiddenTexts = new Entity[4][];
		hiddenTexts[0] = hiddenText0?.ToArray();
		hiddenTexts[1] = hiddenText1?.ToArray();
		hiddenTexts[2] = hiddenText2?.ToArray();
		hiddenTexts[3] = hiddenText3?.ToArray();

		// hide all hiddenText
		for (int i = 0; i < 4; ++i)
		{
			foreach (Entity text in hiddenTexts[i])
			{
				text?.SetActive(false);
			}
		}

		// we'll only use the 2D array during runtime
		hiddenText0 = hiddenText1 = hiddenText2 = hiddenText3 = null;
	}
}





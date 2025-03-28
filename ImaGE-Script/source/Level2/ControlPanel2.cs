﻿using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using static Dialogue;

public class ControlPanel2 : Entity
{
    public PlayerInteraction playerInteraction;
    public Entity UVLight;
    public Entity[] hiddenText0;
    public Entity[] hiddenText1;
    public Entity[] hiddenText2;
    public Entity[] hiddenText3;
    public Entity[] defaultHiddenText;
    public Entity odysseusStatue;
    public Entity zeusStatue;
    public Entity poseidonStatue;
    public Entity athenaStatue;
    public Entity hermesStatue;

    private Entity[][] hiddenTexts;
    public Entity playerCamera;
    public Entity controlPanelCamera;
    public PlayerMove playerMove;
    public float rotationSpeed = 50.0f;

    public float minVerticalRotation = -17.0f;
    public float maxVerticalRotation = 25.0f;
    public float minHorizontalRotation = -36.0f;
    public float maxHorizontalRotation = 32.0f;

    private bool uvLightEnabled = false;

    public Entity LeftClickControlPanelUI;

    public enum StatueType
    {
        ODYSSEUS = 0,
        ZEUS,
        ATHENA,
        POSEIDON,
        HERMES
    }
    private StatueType currStatue = StatueType.ODYSSEUS;

    private enum State
    {
        DEFAULT,
        CLOSED,
        OPEN,
        CONTROL_PANEL,
        UV_LIGHT
    }

    private State currState = State.DEFAULT;
    private bool areLightsOn = true;
    private bool defaultStateActive = true;

    public ControlPanel2() : base() { }

    void Start()
    {
        if (playerMove == null)
        {
            Debug.LogError("[ControlPanel2.cs] PlayerMove Script Entity not found!");
            return;
        }

        InitHiddenTexts();


        odysseusStatue.SetActive(true);
        SetDefaultHiddenText(true);

        // Hide all other statues
        zeusStatue.SetActive(false);
        poseidonStatue.SetActive(false);
        athenaStatue.SetActive(false);
        hermesStatue.SetActive(false);

        UVLight.SetActive(false);

        // Initialize the interaction UI
        if (LeftClickControlPanelUI == null)
        {
            Debug.LogError("[ControlPanel2.cs] Left Click Control PanelUI Entity not found!");
            return;
        }

        LeftClickControlPanelUI.SetActive(false); // Hide prompt at the start
    }

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        bool isPanelHit = playerInteraction.RayHitString == "ControlPanel";
        bool isDoorHit = playerInteraction.RayHitString == "CPDoor";
        bool isInteractable = isPanelHit || isDoorHit;


        LeftClickControlPanelUI.SetActive(isInteractable);



        switch (currState)
        {
            case State.DEFAULT:
                break;

            case State.OPEN:
                //bool mouseClicked = Input.GetMouseButtonTriggered(0);
                //bool isPanelHit = playerInteraction.RayHitString == "ControlPanel";

                if (mouseClicked && isPanelHit)
                {
                    Debug.Log("HIT");
                    InternalCalls.PlaySound(mEntityID, "UVLight");
                    SetControlPanelCameraAsMain();
                    playerMove.FreezePlayer();

                    if (!areLightsOn)
                    {
                        SetHiddenText(currStatue, true);
                        UVLight.SetActive(true);
                        uvLightEnabled = true;
                        currState = State.UV_LIGHT;
                    }
                    else
                    {
                        currState = State.CONTROL_PANEL;
                    }
                }
                break;

            case State.CONTROL_PANEL:
                if (Input.GetKeyTriggered(KeyCode.ESCAPE))
                {
                    playerMove.UnfreezePlayer();
                    SetPlayerCameraAsMain();
                    currState = State.OPEN;
                }
                break;

            case State.UV_LIGHT:
                HandleUVLightControls();

                // Only show hidden text if the room lights are off.
                if (!areLightsOn)
                {
                    if (defaultStateActive)
                    {
                        SetDefaultHiddenText(true);
                    }
                    else
                    {
                        SetHiddenText(currStatue, true);
                    }
                }
                else
                {
                    // If the room lights are on, hide any hidden text.
                    SetDefaultHiddenText(false);
                    SetHiddenText(currStatue, false);
                }

                if (Input.GetKeyTriggered(KeyCode.ESCAPE))
                {
                    playerMove.UnfreezePlayer();
                    SetPlayerCameraAsMain();
                    currState = State.OPEN;
                }
                break;


            case State.CLOSED:
                break;
            default:
                break;
        }
    }

    private void HandleUVLightControls()
    {
        Vector3 currentRotation = InternalCalls.GetRotationEuler(UVLight.mEntityID);
        float deltaTime = Time.deltaTime;
        bool playerInput = false;

        if (Input.GetKeyDown(KeyCode.W))
        {
            currentRotation.X += rotationSpeed * deltaTime;
            playerInput = true;
        }
        if (Input.GetKeyDown(KeyCode.S))
        {
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

        if (playerInput)
        {
            currentRotation.X = Mathf.Clamp(currentRotation.X, minVerticalRotation, maxVerticalRotation);
            currentRotation.Y = Mathf.Clamp(currentRotation.Y, minHorizontalRotation, maxHorizontalRotation);
            InternalCalls.SetRotationEuler(UVLight.mEntityID, ref currentRotation);
        }
    }

    public void SwitchMode(StatueType statue)
    {
        if (defaultStateActive)
        {
            SetDefaultHiddenText(false);
            odysseusStatue.SetActive(false);
            defaultStateActive = false;
        }

        DisableAllStatues();
        DisableAllHiddenTexts();
        switch (statue)
        {
            case StatueType.ODYSSEUS:
                odysseusStatue.SetActive(true);
                break;
            case StatueType.ZEUS:
                zeusStatue.SetActive(true);
                break;
            case StatueType.POSEIDON:
                poseidonStatue.SetActive(true);
                break;
            case StatueType.ATHENA:
                athenaStatue.SetActive(true);
                break;
            case StatueType.HERMES:
                hermesStatue.SetActive(true);
                break;
            default:
                break;
        }

        currStatue = statue;


        if (uvLightEnabled)
        {
            SetHiddenText(currStatue, true);
            UVLight.SetActive(true);
        }
    }

    public void LightsToggled(bool isOn)
    {
        areLightsOn = isOn;
        if (isOn)
        {
            // If the lights are on, hide the hidden text immediately
            if (defaultStateActive)
            {
                SetDefaultHiddenText(false);
            }
            else
            {
                SetHiddenText(currStatue, false);
            }
            uvLightEnabled = false; // Optionally reset this flag if it’s used only when lights are off
        }
    }

    private void DisableAllHiddenTexts()
    {
        // Disable default hidden text first.
        SetDefaultHiddenText(false);

        // Loop over all hidden text arrays and disable each entity.
        if (hiddenTexts != null)
        {
            foreach (var group in hiddenTexts)
            {
                if (group != null)
                {
                    foreach (Entity text in group)
                    {
                        text?.SetActive(false);
                    }
                }
            }
        }
    }
    public void Unlock()
    {
        if (defaultStateActive)
        {
            SetDefaultHiddenText(false);
            odysseusStatue.SetActive(false);
            defaultStateActive = false;
        }

        currState = State.OPEN;
        SetActive(true);
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
        // Skip hidden text for Odysseus.
        if (statue == StatueType.ODYSSEUS)
        {
            return;
        }

        int index = -1;
        // Remap the statue to a hidden text index:
        // We'll assume:
        // Hermes -> hiddenTexts[0]
        // Zeus -> hiddenTexts[1]
        // Athena -> hiddenTexts[2]
        // Poseidon -> hiddenTexts[3]
        switch (statue)
        {
            case StatueType.HERMES:
                index = 0;
                break;
            case StatueType.ZEUS:
                index = 1;
                break;
            case StatueType.ATHENA:
                index = 2;
                break;
            case StatueType.POSEIDON:
                index = 3;
                break;
            default:
                Debug.LogWarning("SetHiddenText: Unexpected statue type: " + statue);
                return;
        }

        if (hiddenTexts != null && index < hiddenTexts.Length)
        {
            foreach (Entity text in hiddenTexts[index])
            {
                text?.SetActive(active);
            }
        }
        else
        {
            Debug.LogWarning("SetHiddenText: Index out of bounds for hiddenTexts.");
        }
    }


    public void SetDefaultHiddenText(bool active)
    {
        foreach (Entity text in defaultHiddenText)
        {
            text?.SetActive(active);
        }
    }

    public void DisableAllStatues()
    {
        zeusStatue.SetActive(false);
        poseidonStatue.SetActive(false);
        athenaStatue.SetActive(false);
        hermesStatue.SetActive(false);
    }

    private void InitHiddenTexts()
    {
        hiddenTexts = new Entity[4][];
        hiddenTexts[0] = hiddenText0?.ToArray();
        hiddenTexts[1] = hiddenText1?.ToArray();
        hiddenTexts[2] = hiddenText2?.ToArray();
        hiddenTexts[3] = hiddenText3?.ToArray();

        for (int i = 0; i < 4; ++i)
        {
            foreach (Entity text in hiddenTexts[i])
            {
                text?.SetActive(false);
            }
        }

        hiddenText0 = hiddenText1 = hiddenText2 = hiddenText3 = null;
    }
}
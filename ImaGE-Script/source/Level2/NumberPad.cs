﻿using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Numerics;

public class NumberPad : Entity
{
    public PlayerInteraction playerInteraction;
    public PlayerMove playerMove;
    public Entity twinDoors;
    public Entity interactWithKeypadUI;
    public Entity keypadInstructionsUI;
    public Entity keypadUI;
    public Entity keypadTextBox;
    public Entity enterButton;
    public Entity backButton;
    private Random random = new Random();

    // Number buttons
    public Entity button0;
    public Entity button1, button2, button3;
    public Entity button4, button5, button6;
    public Entity button7, button8, button9;

    private Dictionary<string, Entity> numberButtons;
    private Dictionary<string, SafeButtons> buttonScripts;
    private string entityTag;
    private string typedCode = "";
    private string correctCode = "2862";
    private bool keypadActive = false;
    private float lastPressTime = 0f;
    private float inputDelay = 0.3f; // Prevents multiple rapid inputs

    private enum State { LOCKED, KEYPAD_UI, UNLOCKED }
    private State currState = State.LOCKED;

    void Start()
    {
        interactWithKeypadUI?.SetActive(false);
        keypadUI?.SetActive(false);
        keypadInstructionsUI?.SetActive(false);
        keypadTextBox?.SetActive(false);
        enterButton?.SetActive(false);
        backButton?.SetActive(false);

        numberButtons = new Dictionary<string, Entity>
        {
            {"0", button0 }, { "1", button1 }, { "2", button2 }, { "3", button3 },
            { "4", button4 }, { "5", button5 }, { "6", button6 },
            { "7", button7 }, { "8", button8 }, { "9", button9 }
        };

        buttonScripts = new Dictionary<string, SafeButtons>();
        foreach (var key in numberButtons.Keys)
        {
            buttonScripts[key] = numberButtons[key].FindScript<SafeButtons>();
            numberButtons[key]?.SetActive(false);
        }

        buttonScripts["Enter"] = enterButton.FindScript<SafeButtons>();
        buttonScripts["Back"] = backButton.FindScript<SafeButtons>();
        entityTag = InternalCalls.GetTag(mEntityID);

        enterButton?.SetActive(false);
        backButton?.SetActive(false);
    }

    void Update()
    {
        switch (currState)
        {
            case State.LOCKED:
                KeypadInteraction();
                break;
            case State.KEYPAD_UI:
                KeypadUIMode();
                break;
            case State.UNLOCKED:
                twinDoors.FindScript<TheTwinDoors>().UnlockDoors();
                Destroy(this);
                break;
        }
    }

    private void KeypadInteraction()
    {
        bool isKeypadHit = playerInteraction.RayHitString == entityTag;

        if (Input.GetMouseButtonTriggered(0) && isKeypadHit && !keypadActive)
        {
            keypadActive = true;
            interactWithKeypadUI.SetActive(false);
            OpenKeypadUI();
            InternalCalls.PlaySound(mEntityID, "SafeInteract"); // ✅ Matches Safe.cs (play when interacting)
        }

        interactWithKeypadUI.SetActive(isKeypadHit && !keypadActive);
    }

    private void KeypadUIMode()
    {
        if (!keypadUI.IsActive()) return;
        SetAllButtonsInactive();

        foreach (var key in numberButtons.Keys)
        {
            if (buttonScripts[key].IsVisible)
            {
                numberButtons[key].SetActive(true);
                if (buttonScripts[key].TriggerButton)
                {
                    buttonScripts[key].TriggerButton = false;
                    AppendDigit(key);
                }
            }
        }

        if (buttonScripts["Enter"].IsVisible)
        {
            enterButton.SetActive(true);
            if (buttonScripts["Enter"].TriggerButton)
            {
                buttonScripts["Enter"].TriggerButton = false;
                EnterButton();
            }
        }

        if (buttonScripts["Back"].IsVisible)
        {
            backButton.SetActive(true);
            if (buttonScripts["Back"].TriggerButton)
            {
                buttonScripts["Back"].TriggerButton = false;
                BackButton();
            }
        }

        if (Input.GetKeyTriggered(KeyCode.ESCAPE))
        {
            CloseKeypadUI();
        }
    }

    private void AppendDigit(string digit)
    {
        if (Time.gameTime - lastPressTime < inputDelay) return;
        lastPressTime = Time.gameTime;

        if (typedCode.Length < 4)
        {
            typedCode += digit;
            InternalCalls.SetText(keypadTextBox.mEntityID, typedCode);
            PlayRandomKeypadButtonSound(); 
        }
    }

    private void EnterButton()
    {
        if (typedCode == correctCode)
        {
            InternalCalls.PlaySound(mEntityID, "SafeUnlock"); 
          //Console.WriteLine("Correct code entered!");
            CloseKeypadUI();
            currState = State.UNLOCKED;
            entityTag = null;
        }
        else
        {
            InternalCalls.PlaySound(mEntityID, "WrongInput"); 
          //Console.WriteLine("Incorrect code, try again.");
            ClearCode();
        }
    }

    private void BackButton()
    {
        if (Time.gameTime - lastPressTime < inputDelay) return;
        lastPressTime = Time.gameTime;

        if (typedCode.Length > 0)
        {
            typedCode = typedCode.Substring(0, typedCode.Length - 1);
            InternalCalls.SetText(keypadTextBox.mEntityID, typedCode);
            InternalCalls.PlaySound(mEntityID, "KeypadBackspace"); // ✅ Matches Safe.cs (play when backspacing)
        }
    }

    private void OpenKeypadUI()
    {
        keypadActive = true;
        playerMove.FreezePlayer();
        keypadUI.SetActive(true);
        keypadTextBox.SetActive(true);
        keypadInstructionsUI.SetActive(true);
        enterButton.SetActive(true);
        backButton.SetActive(true);
        InternalCalls.ShowCursor();
        ResetButtonStates();
        currState = State.KEYPAD_UI;
    }

    private void CloseKeypadUI()
    {
        keypadActive = false;
        playerMove.UnfreezePlayer();
        keypadUI.SetActive(false);
        keypadTextBox.SetActive(false);
        keypadInstructionsUI.SetActive(false);
        enterButton.SetActive(false);
        backButton.SetActive(false);
        interactWithKeypadUI.SetActive(false);
        SetAllButtonsInactive();
        InternalCalls.HideCursor();
        currState = State.LOCKED;
    }

    private void SetAllButtonsInactive()
    {
        foreach (var button in numberButtons.Values)
        {
            button?.SetActive(false);
        }
        enterButton?.SetActive(false);
        backButton?.SetActive(false);
    }

    private void ClearCode()
    {
        typedCode = "";
        InternalCalls.SetText(keypadTextBox.mEntityID, "");
    }

    private void ResetButtonStates()
    {
        foreach (var key in buttonScripts.Keys)
        {
            buttonScripts[key].IsClicked = false;
            buttonScripts[key].TriggerButton = false;
        }
    }

    private void PlayRandomKeypadButtonSound()
    {
        int soundNumber = random.Next(1, 3); // ✅ Matches Safe.cs (random beep sound)
        string soundName = $"KeypadBeep{soundNumber}";
        InternalCalls.PlaySound(mEntityID, soundName);
    }
}

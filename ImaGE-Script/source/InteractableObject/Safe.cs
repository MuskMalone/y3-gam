using IGE.Utils;
using System;
using System.Collections.Generic;
using System.Numerics;
using static Dialogue;

public class Safe : Entity
{
  // Script to be placed in the Safe Entity (Parent)
  public PlayerInteraction playerInteraction;
  public Entity interactWithSafeUI;
  public Entity safeInstructionsUI;
  public Entity keyPadUI;
  public Entity safeTextBox; // Entity with text component to display the keyed in characters
  public PlayerMove playerMove;
  public Entity safeDoorPart;
  private Random random = new Random();
  public Inventory inventory;

  // Workaround for broken Entity[]
  public Entity ABC;
  public Entity DEF;
  public Entity GHI;
  public Entity JKL;
  public Entity MNO;
  public Entity PQRS;
  public Entity TUV;
  public Entity WXYZ;
  public Entity Enter;
  public Entity Back;
  public Entity doorPivot;  // for door opening anim

  private SafeButtons ABCButtonScript;
  private SafeButtons DEFButtonScript;
  private SafeButtons GHIButtonScript;
  private SafeButtons JKLButtonScript;
  private SafeButtons MNOButtonScript;
  private SafeButtons PQRSButtonScript;
  private SafeButtons TUVButtonScript;
  private SafeButtons WXYZButtonScript;
  private SafeButtons EnterButtonScript;
  private SafeButtons BackButtonScript;

  static private readonly string openSafeAnim = "SafeOpen";
  private bool safeInteraction = false;
  private bool safeUIActive = false;

  private Dictionary<string, string[]> letterGroups = new Dictionary<string, string[]>
    {
        { "ABC", new string[] { "A", "B", "C" } },
        { "DEF", new string[] { "D", "E", "F" } },
        { "GHI", new string[] { "G", "H", "I" } },
        { "JKL", new string[] { "J", "K", "L" } },
        { "MNO", new string[] { "M", "N", "O" } },
        { "PQRS", new string[] { "P", "Q", "R", "S" } },
        { "TUV", new string[] { "T", "U", "V" } },
        { "WXYZ", new string[] { "W", "X", "Y", "Z" } }
    };

  private Dictionary<string, int> currentIndex = new Dictionary<string, int>();  // Track letter index per button group
  private float inputDelay = 0.5f;                                               // Shortened input delay to finalize letter
  private string typedText = "";                                                 // Store finalized input
  private string currentButtonGroup = "";                                        // Track the currently active button group
  private float timeSinceLastPress = 0f;                                         // Timer to handle delay for letter selection
  private float lastPressTime = 0f;                                              // Store the time of the last button press
  private bool waitingForDelay = false;

  public bool enterPressed = false;
  public bool correctAnswer = false;

  // Hammer/Crowbar logic
  public Entity crowbarAtSafe;
  public Entity hammerAtSafe;

  public Dialogue dialogue; // Reference to the Dialogue script
  public string[] hammerDialogue = { "I need something to wedge into the gap first!" };
  public string[] crowbarDialogue = { "There must be something I can use to whack it into\nthe hinge." };
  public string[] lastDialogue = { "Alright, now one good whack!" };

  public bool isCrowbarOnSafe;
  public bool isHammerOnSafe;
  public bool isSafeOpenedByCrowbar = false;
  public bool safeSolvedByCrowbar;

  Emotion[] EhammerFirst = { Emotion.Thinking };
  Emotion[] EcrowbarFirst = { Emotion.Surprised };
  Emotion[] EhammerAfterCrowbar = { Emotion.Neutral };

  void Start()
  {
    interactWithSafeUI?.SetActive(false);
    keyPadUI?.SetActive(false);
    safeTextBox?.SetActive(false);
    safeInstructionsUI?.SetActive(false);
    ABC?.SetActive(false);
    DEF?.SetActive(false);
    GHI?.SetActive(false);
    JKL?.SetActive(false);
    MNO?.SetActive(false);
    PQRS?.SetActive(false);
    TUV?.SetActive(false);
    WXYZ?.SetActive(false);
    Enter?.SetActive(false);
    Back?.SetActive(false);

    ABCButtonScript = ABC.FindScript<SafeButtons>();
    DEFButtonScript = DEF.FindScript<SafeButtons>();
    GHIButtonScript = GHI.FindScript<SafeButtons>();
    JKLButtonScript = JKL.FindScript<SafeButtons>();
    MNOButtonScript = MNO.FindScript<SafeButtons>();
    PQRSButtonScript = PQRS.FindScript<SafeButtons>();
    TUVButtonScript = TUV.FindScript<SafeButtons>();
    WXYZButtonScript = WXYZ.FindScript<SafeButtons>();
    EnterButtonScript = Enter.FindScript<SafeButtons>();
    BackButtonScript = Back.FindScript<SafeButtons>();

    foreach (var key in letterGroups.Keys)
    {
      currentIndex[key] = 0;
    }
  }

  void Update()
  {
    if (!isSafeOpenedByCrowbar)
    {
      if (!correctAnswer && !safeInteraction)
      {
        bool isSafeHit = playerInteraction.RayHitString == InternalCalls.GetTag(safeDoorPart.mEntityID);

        //if player tries to use hammer on it first
        if (Input.GetMouseButtonTriggered(0) && isSafeHit && !safeUIActive && 
          inventory.hammerEquipped && !isCrowbarOnSafe && !dialogue.isInDialogueMode)
        {
          dialogue.SetDialogue(hammerDialogue, EhammerFirst);
          //interactWithSafeUI.SetActive(false);
          return;
        }

        // crowbar is equipped and placed on safe
        //insert dialogue: Now i need something to whack this thing 
        else if (Input.GetMouseButtonTriggered(0) && isSafeHit && !safeUIActive &&
          inventory.crowbarEquipped && !isCrowbarOnSafe && !dialogue.isInDialogueMode)
        {
          crowbarAtSafe.SetActive(true);
          //audioManager.Play3DSound(audioManager.crowbar, crowbarAtSafe, audioManager.CrowbarVolume);

          IInventoryItem itemToUse = inventory.GetItemByName("Crowbar");
          Debug.Log("itemToUse" + itemToUse);
          if (itemToUse != null)
          {
            Debug.Log("name" + itemToUse.Name);
            inventory.RemoveItem(itemToUse);
          }

          isCrowbarOnSafe = true;
          dialogue.SetDialogue(crowbarDialogue, EcrowbarFirst);
          //interactWithSafeUI.SetActive(false);
          return;
        }

        // if crowbar is already on the safe but hammer isnt and hammer is equipped
        //insert dialogue Alright now give it a good whack 
        else if (Input.GetMouseButtonTriggered(0) && isSafeHit && !safeUIActive && 
          isCrowbarOnSafe && inventory.hammerEquipped && !isHammerOnSafe && !dialogue.isInDialogueMode)
        {
          hammerAtSafe.SetActive(true);
          //audioManager.Play3DSound(audioManager.hammer, hammerAtSafe, audioManager.HammerVolume);
          IInventoryItem itemToUse = inventory.GetItemByName("Hammer");
          Debug.Log("itemToUse" + itemToUse);
          if (itemToUse != null)
          {
            Debug.Log("name" + itemToUse.Name);
            inventory.RemoveItem(itemToUse); // Remove after use
          }

          isHammerOnSafe = true;
          dialogue.SetDialogue(lastDialogue, EhammerAfterCrowbar); // Send specific dialogue lines for scene 1
          //interactWithSafeUI.SetActive(false);
          return;
        }

        ////if crowbar and hammer are on the safe already
        ////position the crowbar and hammer and safe door on the ground
        else if (Input.GetMouseButtonTriggered(0) && isSafeHit && !safeUIActive && isCrowbarOnSafe && isHammerOnSafe)
        {
          /*
          //move and rotate crowbar
          //crowbarAtSafe.transform.localposition = crowbarNewPos;
          crowbarAtSafe.transform.localPosition = crowbarNewPos;
          crowbarAtSafe.transform.eulerAngles = crowbarNewRot;
          //move and rotate hammer
          hammerAtSafe.transform.localPosition = hammerNewPos;
          hammerAtSafe.transform.eulerAngles = hammerNewRot;
          */
          //interactWithSafeUI.SetActive(false);
          isSafeOpenedByCrowbar = true;
          InternalCalls.PlaySound(mEntityID, "SafeInteract");
          EndSafeUI();
          safeDoorPart.SetActive(false);
          hammerAtSafe.SetActive(false);
          crowbarAtSafe.SetActive(false);
          return;
        }

        else if (Input.GetMouseButtonTriggered(0) && isSafeHit && !safeUIActive && 
          !isHammerOnSafe && !inventory.crowbarEquipped && !inventory.hammerEquipped && !dialogue.isInDialogueMode)
        {
          safeInteraction = true;
          interactWithSafeUI.SetActive(false);
          BeginSafeUI();
          return;
        }

        interactWithSafeUI.SetActive(isSafeHit);
      }
    }

    if (safeUIActive)
    {
      SetAllButtonsInactive();
      if (ABCButtonScript.IsVisible)
      {
        ABC.SetActive(true);
        if (ABCButtonScript.TriggerButton)
        {
          ABCButtonScript.TriggerButton = false;
          PressButton("ABC");
        }
      }

      else if (DEFButtonScript.IsVisible) { 
        DEF.SetActive(true);
        if (DEFButtonScript.TriggerButton)
        {
          DEFButtonScript.TriggerButton = false;
          PressButton("DEF");
        }
      }

      else if (GHIButtonScript.IsVisible) { 
        GHI.SetActive(true);
        if (GHIButtonScript.TriggerButton)
        {
          GHIButtonScript.TriggerButton = false;
          PressButton("GHI");
        }
      }

      else if (JKLButtonScript.IsVisible) { 
        JKL.SetActive(true);
        if (JKLButtonScript.TriggerButton)
        {
          JKLButtonScript.TriggerButton = false;
          PressButton("JKL");
        }
      }

      else if (MNOButtonScript.IsVisible) { 
        MNO.SetActive(true);
        if (MNOButtonScript.TriggerButton)
        {
          MNOButtonScript.TriggerButton = false;
          PressButton("MNO");
        }
      }

      else if (PQRSButtonScript.IsVisible) { 
        PQRS.SetActive(true);
        if (PQRSButtonScript.TriggerButton)
        {
          PQRSButtonScript.TriggerButton = false;
          PressButton("PQRS");
        }
      }

      else if (TUVButtonScript.IsVisible) { 
        TUV.SetActive(true);
        if (TUVButtonScript.TriggerButton)
        {
          TUVButtonScript.TriggerButton = false;
          PressButton("TUV");
        }
      }

      else if (WXYZButtonScript.IsVisible) { 
        WXYZ.SetActive(true);
        if (WXYZButtonScript.TriggerButton)
        {
          WXYZButtonScript.TriggerButton = false;
          PressButton("WXYZ");
        }
      }

      else if (EnterButtonScript.IsVisible)
      {
        Enter.SetActive(true);
        if (EnterButtonScript.TriggerButton)
        {
          EnterButtonScript.TriggerButton = false;
          EnterButton();
        }
      }

      else if (BackButtonScript.IsVisible)
      {
        Back.SetActive(true);
        if (BackButtonScript.TriggerButton)
        {
          BackButtonScript.TriggerButton = false;
          BackButton();
        }
      }

      if (Input.GetKeyTriggered(KeyCode.ESCAPE))
      {
        EndSafeUI();
        safeInteraction = false;
      }

      /*
      if (Input.GetKeyTriggered(KeyCode.BACKSPACE))
      {
        typedText = typedText.Substring(0, typedText.Length - 1);
        InternalCalls.SetText(safeTextBox.mEntityID, typedText);
      }
      */

      if (waitingForDelay)
      {
        if (Time.gameTime - lastPressTime >= inputDelay)
        {
          FinalizeCurrentLetter();
        }
      }

      if (enterPressed)
      {
        if (InternalCalls.GetText(safeTextBox.mEntityID) == "CEREUS")
        {
          Debug.Log("Correct answer");
          correctAnswer = true;
        }
        else
        {
          InternalCalls.PlaySound(mEntityID, "WrongInput");
          correctAnswer = false;
          ClearText();
        }
        enterPressed = false;
      }
      
      if (correctAnswer)
      {
        InternalCalls.PlaySound(mEntityID, "SafeInteract");
        InternalCalls.PlaySound(mEntityID, "SafeUnlock");
        //InternalCalls.PlayAnimation(doorPivot.mEntityID, openSafeAnim);
        EndSafeUI();
        safeDoorPart.SetActive(false);
      }
    }
  }

  private void BeginSafeUI()
  {
    safeUIActive = true;
    playerMove.FreezePlayer();
    keyPadUI.SetActive(true);
    safeTextBox.SetActive(true);
    safeInstructionsUI.SetActive(true);
    InternalCalls.ShowCursor();
  }

  private void EndSafeUI()
  {
    safeUIActive = false;
    playerMove.UnfreezePlayer();
    keyPadUI.SetActive(false);
    safeTextBox.SetActive(false);
    safeInstructionsUI.SetActive(false);
    interactWithSafeUI.SetActive(false);

    SetAllButtonsInactive();
    InternalCalls.HideCursor();
  }

  private void SetAllButtonsInactive()
  {
    ABC?.SetActive(false);
    DEF?.SetActive(false);
    GHI?.SetActive(false);
    JKL?.SetActive(false);
    MNO?.SetActive(false);
    PQRS?.SetActive(false);
    TUV?.SetActive(false);
    WXYZ?.SetActive(false);
    Enter?.SetActive(false);
    Back?.SetActive(false);
  }

  public void PressButton(string buttonGroup)
  {
    PlayRandomKeypadButtonSound();

    if (!letterGroups.ContainsKey(buttonGroup)) return;

    if (currentButtonGroup != buttonGroup && !string.IsNullOrEmpty(currentButtonGroup))
    {
      FinalizeCurrentLetter();
    }

    currentButtonGroup = buttonGroup;
    string currentLetter = letterGroups[buttonGroup][currentIndex[buttonGroup]];
    InternalCalls.SetText(safeTextBox.mEntityID, typedText + currentLetter);

    if (waitingForDelay)
    {
      lastPressTime = Time.gameTime;
    }
    else
    {
      waitingForDelay = true;
      lastPressTime = Time.gameTime;
    }

    currentIndex[buttonGroup] = (currentIndex[buttonGroup] + 1) % letterGroups[buttonGroup].Length;
  }

  private void FinalizeCurrentLetter()
  {
    if (!string.IsNullOrEmpty(currentButtonGroup))
    {
      string finalLetter = letterGroups[currentButtonGroup][(currentIndex[currentButtonGroup] +
        letterGroups[currentButtonGroup].Length - 1) % letterGroups[currentButtonGroup].Length];
      typedText += finalLetter;
      InternalCalls.SetText(safeTextBox.mEntityID, typedText);
      currentIndex[currentButtonGroup] = 0;
      currentButtonGroup = "";
      waitingForDelay = false;
    }
  }

  public void ClearText()
  {
    typedText = "";
    InternalCalls.SetText(safeTextBox.mEntityID, "");
    currentButtonGroup = "";
    foreach (var key in letterGroups.Keys)
    {
      currentIndex[key] = 0;
    }
  }

  public void EnterButton()
  {
    enterPressed = true;
  }
  private void BackButton()
  {
    PlayRandomKeypadButtonSound();
    typedText = typedText.Substring(0, typedText.Length - 1);
    InternalCalls.SetText(safeTextBox.mEntityID, typedText);
  }
  private void PlayRandomKeypadButtonSound()
  {
    int soundNumber = random.Next(1, 2);
    string soundName = $"KeypadBeep{soundNumber}";
    InternalCalls.PlaySound(mEntityID, soundName);
  }
}

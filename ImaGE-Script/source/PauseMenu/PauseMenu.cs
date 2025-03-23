using IGE.Utils;
using System;
using System.Numerics;

public class PauseMenu : Entity
{
  public Entity ResumeButton;
  public Entity MainMenuButton;
  public Entity SettingsButton;
  public Transition transition;
 public SettingsPage settingsPage;
  public float transitionDuration = 0.5f;

  private float transitionTimer = 0f;
  private PauseMenuButtons ResumeButtonScript;
  private PauseMenuButtons MainMenuButtonScript;
  private PauseMenuButtons SettingsButtonScript;

  public PlayerMove playerMove;

  private float TargetZMenu = 2f;
  private float TargetZButton = 3f;

  private float OriginalTargetZMenu = -2f;
  private float OriginalTargetZButton = -2f;

  private enum State
  {
    NONE,
    RESUME,
    MAIN_MENU,
    SETTINGS,
    PAUSE
  }
  private State currState = State.NONE;

  void Start()
  {
    Console.WriteLine("1: Hiding ResumeButton.");
    ResumeButton?.SetActive(false);

    Console.WriteLine("2: Hiding MainMenuButton.");
    MainMenuButton?.SetActive(false);

    Console.WriteLine("3: Hiding SettingsButton.");
    SettingsButton?.SetActive(false);

    Console.WriteLine("4: Finding script on ResumeButton.");
    ResumeButtonScript = ResumeButton.FindScript<PauseMenuButtons>();

    Console.WriteLine("5: Finding script on MainMenuButton.");
    MainMenuButtonScript = MainMenuButton.FindScript<PauseMenuButtons>();

    Console.WriteLine("6: Finding script on SettingsButton.");
    SettingsButtonScript = SettingsButton.FindScript<PauseMenuButtons>();

    Console.WriteLine("7: Getting original menu position.");
    Vector3 originalMenu = InternalCalls.GetPosition(mEntityID);

    Console.WriteLine($"8: Original Menu Position -> X: {originalMenu.X}, Y: {originalMenu.Y}, Z: {originalMenu.Z}");
    Vector3 newPosMenu = new Vector3(originalMenu.X, originalMenu.Y, OriginalTargetZMenu);

    Console.WriteLine($"9: Setting Menu Position to -> X: {newPosMenu.X}, Y: {newPosMenu.Y}, Z: {newPosMenu.Z}");
    InternalCalls.SetPosition(mEntityID, ref newPosMenu);

    Console.WriteLine("10: Getting original ResumeButton position.");
    Vector3 originalResume = InternalCalls.GetPosition(ResumeButtonScript.mEntityID);

    Console.WriteLine($"11: Original ResumeButton Position -> X: {originalResume.X}, Y: {originalResume.Y}, Z: {originalResume.Z}");
    Vector3 newPosResume = new Vector3(originalResume.X, originalResume.Y, OriginalTargetZButton);

    Console.WriteLine($"12: Setting ResumeButton Position to -> X: {newPosResume.X}, Y: {newPosResume.Y}, Z: {newPosResume.Z}");
    InternalCalls.SetPosition(ResumeButtonScript.mEntityID, ref newPosResume);

    Console.WriteLine("13: Getting original SettingsButton position.");
    Vector3 originalSettings = InternalCalls.GetPosition(SettingsButtonScript.mEntityID);

    Console.WriteLine($"14: Original SettingsButton Position -> X: {originalSettings.X}, Y: {originalSettings.Y}, Z: {originalSettings.Z}");
    Vector3 newPosSettings = new Vector3(originalSettings.X, originalSettings.Y, OriginalTargetZButton);

    Console.WriteLine($"15: Setting SettingsButton Position to -> X: {newPosSettings.X}, Y: {newPosSettings.Y}, Z: {newPosSettings.Z}");
    InternalCalls.SetPosition(SettingsButtonScript.mEntityID, ref newPosSettings);

    Console.WriteLine("16: Getting original MainMenuButton position.");
    Vector3 originalMainMenu = InternalCalls.GetPosition(MainMenuButtonScript.mEntityID);

    Console.WriteLine($"17: Original MainMenuButton Position -> X: {originalMainMenu.X}, Y: {originalMainMenu.Y}, Z: {originalMainMenu.Z}");
    Vector3 newPosMainMenu = new Vector3(originalMainMenu.X, originalMainMenu.Y, OriginalTargetZButton);

    Console.WriteLine($"18: Setting MainMenuButton Position to -> X: {newPosMainMenu.X}, Y: {newPosMainMenu.Y}, Z: {newPosMainMenu.Z}");
    InternalCalls.SetPosition(MainMenuButtonScript.mEntityID, ref newPosMainMenu);
  }


  void Update()
  {
    if (!InternalCalls.GetIsPaused() && InternalCalls.IsKeyTriggered(KeyCode.P))
    {
      //InternalCalls.TakeScreenShot("CurrentView", (int)Input.screenWidth, (int)Input.screenHeight);
      transition.StartTransitionInOut(transitionDuration, Transition.TransitionType.FADE);
      currState = State.PAUSE;
      InternalCalls.PauseGame();
      playerMove.FreezePlayer();
    }

    if (InternalCalls.GetIsPaused())
    {
      SetAllButtonsInactive();

      /*
      if (InternalCalls.IsKeyTriggered(KeyCode.P))
      {
        ResumeGame();
      }
      */

      switch (currState)
      {
        case State.NONE:
          {
            if (ResumeButtonScript.IsVisible)
            {
              ResumeButton.SetActive(true);
              if (ResumeButtonScript.TriggerButton)
              {
                ResumeButtonScript.TriggerButton = false;

                // Resume Game
                transition.StartTransitionInOut(transitionDuration, Transition.TransitionType.FADE);
                currState = State.RESUME;
              }
            }

            else if (MainMenuButtonScript.IsVisible)
            {
              MainMenuButton.SetActive(true);
              if (MainMenuButtonScript.TriggerButton)
              {
                MainMenuButtonScript.TriggerButton = false;

                // Go to Main Menu
                transition.StartTransitionInOut(transitionDuration, Transition.TransitionType.FADE);
                currState = State.MAIN_MENU;

              }
            }

            else if (SettingsButtonScript.IsVisible)
            {
              SettingsButton.SetActive(true);
              if (SettingsButtonScript.TriggerButton)
              {
                SettingsButtonScript.TriggerButton = false;

                // Go to Settings
                transition.StartTransitionInOut(transitionDuration, Transition.TransitionType.FADE);
                currState = State.SETTINGS;
              }
            }
            break;
          }
        case State.PAUSE:
          {
            transitionTimer += Time.deltaTime;
            if (transitionTimer >= transitionDuration)
            {
              Reset();
              PauseGame();
            }
            break;
          }

        case State.RESUME:
          {
            Debug.Log("RESUME???");
            transitionTimer += Time.deltaTime;
            if (transitionTimer >= transitionDuration)
            {
              Reset();
              Debug.Log("RESUME!!!");
              ResumeGame();
            }
            break;
          }

        case State.MAIN_MENU:
          {
            transitionTimer += Time.deltaTime;
            if (transitionTimer >= transitionDuration)
            {
              Reset();
              InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\mainmenu.scn");
            }
            break;
          }

        case State.SETTINGS:
          {
            transitionTimer += Time.deltaTime;
            if (transitionTimer >= transitionDuration)
            {
              Reset();
              DisplaySettings();
              // InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\Settings.scn");
            }
            break;
          }

        default:
          break;
      }
    }
  }

  private void SetAllButtonsInactive()
  {
    ResumeButton?.SetActive(false);
    MainMenuButton?.SetActive(false);
    SettingsButton?.SetActive(false);
  }

  public void GoBackToPauseMenu()
  {
    InternalCalls.PauseGame();
    Reset();
    PauseGame();
  }

  private void PauseGame()
  {
    SetActive(true);
    InternalCalls.ShowCursor();
    Debug.Log("Game Paused");

    Vector3 originalMenu = InternalCalls.GetPosition(mEntityID);
    Vector3 newPosMenu = new Vector3(originalMenu.X, originalMenu.Y, TargetZMenu);
    InternalCalls.SetPosition(mEntityID, ref newPosMenu);

    Vector3 originalResume = InternalCalls.GetPosition(ResumeButtonScript.mEntityID);
    Vector3 newPosResume = new Vector3(originalResume.X, originalResume.Y, TargetZButton);
    InternalCalls.SetPosition(ResumeButtonScript.mEntityID, ref newPosResume);

    Vector3 originalSettings = InternalCalls.GetPosition(SettingsButtonScript.mEntityID);
    Vector3 newPosSettings = new Vector3(originalSettings.X, originalSettings.Y, TargetZButton);
    InternalCalls.SetPosition(SettingsButtonScript.mEntityID, ref newPosSettings);

    Vector3 originalMainMenu = InternalCalls.GetPosition(MainMenuButtonScript.mEntityID);
    Vector3 newPosMainMenu = new Vector3(originalMainMenu.X, originalMainMenu.Y, TargetZButton);
    InternalCalls.SetPosition(MainMenuButtonScript.mEntityID, ref newPosMainMenu);
  }

  private void DisplaySettings()
  {
    settingsPage.SetActive(true);
    settingsPage.SetPageActive();
    Reset();
    SetActive(false);
 
    SetAllButtonsInactive();
    Vector3 originalMenu = InternalCalls.GetPosition(mEntityID);
    Vector3 newPosMenu = new Vector3(originalMenu.X, originalMenu.Y, OriginalTargetZMenu);
    InternalCalls.SetPosition(mEntityID, ref newPosMenu);

    Vector3 originalResume = InternalCalls.GetPosition(ResumeButtonScript.mEntityID);
    Vector3 newPosResume = new Vector3(originalResume.X, originalResume.Y, OriginalTargetZButton);
    InternalCalls.SetPosition(ResumeButtonScript.mEntityID, ref newPosResume);

    Vector3 originalSettings = InternalCalls.GetPosition(SettingsButtonScript.mEntityID);
    Vector3 newPosSettings = new Vector3(originalSettings.X, originalSettings.Y, OriginalTargetZButton);
    InternalCalls.SetPosition(SettingsButtonScript.mEntityID, ref newPosSettings);

    Vector3 originalMainMenu = InternalCalls.GetPosition(MainMenuButtonScript.mEntityID);
    Vector3 newPosMainMenu = new Vector3(originalMainMenu.X, originalMainMenu.Y, OriginalTargetZButton);
    InternalCalls.SetPosition(MainMenuButtonScript.mEntityID, ref newPosMainMenu);
  }



  private void ResumeGame()
  {
    InternalCalls.ResumeGame();
    playerMove.UnfreezePlayer();
    SetActive(false);
    SetAllButtonsInactive();
    InternalCalls.HideCursor();
    Debug.Log("Game Resume");

    Vector3 originalMenu = InternalCalls.GetPosition(mEntityID);
    Vector3 newPosMenu = new Vector3(originalMenu.X, originalMenu.Y, OriginalTargetZMenu);
    InternalCalls.SetPosition(mEntityID, ref newPosMenu);

    Vector3 originalResume = InternalCalls.GetPosition(ResumeButtonScript.mEntityID);
    Vector3 newPosResume = new Vector3(originalResume.X, originalResume.Y, OriginalTargetZButton);
    InternalCalls.SetPosition(ResumeButtonScript.mEntityID, ref newPosResume);

    Vector3 originalSettings = InternalCalls.GetPosition(SettingsButtonScript.mEntityID);
    Vector3 newPosSettings = new Vector3(originalSettings.X, originalSettings.Y, OriginalTargetZButton);
    InternalCalls.SetPosition(SettingsButtonScript.mEntityID, ref newPosSettings);

    Vector3 originalMainMenu = InternalCalls.GetPosition(MainMenuButtonScript.mEntityID);
    Vector3 newPosMainMenu = new Vector3(originalMainMenu.X, originalMainMenu.Y, OriginalTargetZButton);
    InternalCalls.SetPosition(MainMenuButtonScript.mEntityID, ref newPosMainMenu);
  }


  private void Reset()
  {
    transitionTimer = 0f;
    currState = State.NONE;
  }
}

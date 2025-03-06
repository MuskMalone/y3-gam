using IGE.Utils;

public class PauseMenu : Entity
{
  public Entity ResumeButton;
  public Entity MainMenuButton;
  public Entity SettingsButton;

  private PauseMenuButtons ResumeButtonScript;
  private PauseMenuButtons MainMenuButtonScript;
  private PauseMenuButtons SettingsButtonScript;

  public bool IsPaused = false;

  void Start()
  {
    ResumeButton?.SetActive(false);
    MainMenuButton?.SetActive(false);
    SettingsButton?.SetActive(false);

    ResumeButtonScript = ResumeButton.FindScript<PauseMenuButtons>();
    MainMenuButtonScript = MainMenuButton.FindScript<PauseMenuButtons>();
    SettingsButtonScript = SettingsButton.FindScript<PauseMenuButtons>();
  }

  void Update()
  {
    if (!IsPaused && InternalCalls.IsKeyTriggered(KeyCode.P)) {
      PauseGame();
    }

    if (IsPaused)
    {
      SetAllButtonsInactive();

      /*
      if (InternalCalls.IsKeyTriggered(KeyCode.P))
      {
        ResumeGame();
      }
      */

      if (ResumeButtonScript.IsVisible)
      {
        ResumeButton.SetActive(true);
        if (ResumeButtonScript.TriggerButton)
        {
          ResumeButtonScript.TriggerButton = false;

          // Resume Game
          ResumeGame();
        }
      }

      else if (MainMenuButtonScript.IsVisible)
      {
        MainMenuButton.SetActive(true);
        if (MainMenuButtonScript.TriggerButton)
        {
          MainMenuButtonScript.TriggerButton = false;

          // Go to Main Menu
          InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\mainmenu.scn");
        }
      }

      else if (SettingsButtonScript.IsVisible)
      {
        SettingsButton.SetActive(true);
        if (SettingsButtonScript.TriggerButton)
        {
          SettingsButtonScript.TriggerButton = false;

          // Go to Settings

        }
      }
    }
  }

  private void SetAllButtonsInactive()
  {
    ResumeButton?.SetActive(false);
    MainMenuButton?.SetActive(false);
    SettingsButton?.SetActive(false);
  }

  private void PauseGame()
  {
    IsPaused = true;
    SetActive(true);
    InternalCalls.ShowCursor();
    Debug.Log("Game Paused");
  }

  private void ResumeGame()
  {
    IsPaused = false;
    SetActive(false);
    SetAllButtonsInactive();
    InternalCalls.HideCursor();
    Debug.Log("Game Resume");
  }
}

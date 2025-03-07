using IGE.Utils;
using System.Numerics;

public class PauseMenu : Entity
{
  public Entity ResumeButton;
  public Entity MainMenuButton;
  public Entity SettingsButton;

  private PauseMenuButtons ResumeButtonScript;
  private PauseMenuButtons MainMenuButtonScript;
  private PauseMenuButtons SettingsButtonScript;

  public bool IsPaused = false;

  private float TargetZMenu = 2f;
  private float TargetZButton = 3f;

  private float OriginalTargetZMenu = -2f;
  private float OriginalTargetZButton = -2f;

  void Start()
  {
    ResumeButton?.SetActive(false);
    MainMenuButton?.SetActive(false);
    SettingsButton?.SetActive(false);

    ResumeButtonScript = ResumeButton.FindScript<PauseMenuButtons>();
    MainMenuButtonScript = MainMenuButton.FindScript<PauseMenuButtons>();
    SettingsButtonScript = SettingsButton.FindScript<PauseMenuButtons>();

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

  private void ResumeGame()
  {
    IsPaused = false;
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
}

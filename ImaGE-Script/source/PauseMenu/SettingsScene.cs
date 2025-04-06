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
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using static System.TimeZoneInfo;

public class SettingsScene : Entity
{
  private bool isPageActive = false;

  public SettingsButtons VolumeSlider;
  public SettingsButtons SFXSlider;
  public SettingsButtons BrightnessSlider;
  public PauseMenuButtons BackButton;
  public Transition transition;
  public float transitionDuration = 0.5f;
  private bool isTransitioning = false;
  private float transitionTimer = 0f;
  public Entity SFXText;
  public Entity BGMText;
  public Entity GammaText;
  public Entity SFXBorder;
  public Entity BGMBorder;
  public Entity GammaBorder;

  public float CanvasmaxX = 0.5f;
  public float CanvasminX = -0.5f;
  public float maxX = -0.144f;
  public float minX = -0.292f;

  public float TargetZMenu = 2f;
  public float TargetZButton = 3f;

  public float OriginalTargetZMenu = -2f;
  public float OriginalTargetZButton = -2f;

  SettingsScene() : base()
  {

  }
  // Start is called before the first frame update
  void Start()
  {
    InternalCalls.ShowCursor();
  //Console.WriteLine("7: Getting original menu position.");
    Vector3 originalMenu = InternalCalls.GetPosition(mEntityID);
  //Console.WriteLine($"8: Original Menu Position -> X: {originalMenu.X}, Y: {originalMenu.Y}, Z: {originalMenu.Z}");
    Vector3 newPosMenu = new Vector3(originalMenu.X, originalMenu.Y, OriginalTargetZMenu);
  //Console.WriteLine($"9: Setting Menu Position to -> X: {newPosMenu.X}, Y: {newPosMenu.Y}, Z: {newPosMenu.Z}");
    InternalCalls.SetPosition(mEntityID, ref newPosMenu);

  //Console.WriteLine("10: Getting original ResumeButton position.");
    Vector3 originalVol = InternalCalls.GetPosition(VolumeSlider.mEntityID);
  //Console.WriteLine($"11: Original ResumeButton Position -> X: {originalVol.X}, Y: {originalVol.Y}, Z: {originalVol.Z}");
    Vector3 newPosVol = new Vector3(originalVol.X, originalVol.Y, OriginalTargetZButton);
  //Console.WriteLine($"12: Setting ResumeButton Position to -> X: {newPosVol.X}, Y: {newPosVol.Y}, Z: {newPosVol.Z}");
    InternalCalls.SetPosition(VolumeSlider.mEntityID, ref newPosVol);

  //Console.WriteLine("13: Getting original SettingsButton position.");
    Vector3 originalBright = InternalCalls.GetPosition(BrightnessSlider.mEntityID);
  //Console.WriteLine($"14: Original SettingsButton Position -> X: {originalBright.X}, Y: {originalBright.Y}, Z: {originalBright.Z}");
    Vector3 newPosSettings = new Vector3(originalBright.X, originalBright.Y, OriginalTargetZButton);
  //Console.WriteLine($"15: Setting SettingsButton Position to -> X: {newPosSettings.X}, Y: {newPosSettings.Y}, Z: {newPosSettings.Z}");
    InternalCalls.SetPosition(BrightnessSlider.mEntityID, ref newPosSettings);


  //Console.WriteLine("10: Getting original ResumeButton position.");
    Vector3 originalSFX = InternalCalls.GetPosition(SFXSlider.mEntityID);
  //Console.WriteLine($"11: Original ResumeButton Position -> X: {originalSFX.X}, Y: {originalSFX.Y}, Z: {originalSFX.Z}");
    Vector3 newPosSFX = new Vector3(originalSFX.X, originalSFX.Y, OriginalTargetZButton);
  //Console.WriteLine($"12: Setting ResumeButton Position to -> X: {newPosSFX.X}, Y: {newPosSFX.Y}, Z: {newPosSFX.Z}");
    InternalCalls.SetPosition(SFXSlider.mEntityID, ref newPosSFX);



  //Console.WriteLine("16: Getting original MainMenuButton position.");
    Vector3 originalBack = InternalCalls.GetPosition(BackButton.mEntityID);
  //Console.WriteLine($"17: Original MainMenuButton Position -> X: {originalBack.X}, Y: {originalBack.Y}, Z: {originalBack.Z}");
    Vector3 newPosMainMenu = new Vector3(originalBack.X, originalBack.Y, OriginalTargetZButton);
  //Console.WriteLine($"18: Setting MainMenuButton Position to -> X: {newPosMainMenu.X}, Y: {newPosMainMenu.Y}, Z: {newPosMainMenu.Z}");
    InternalCalls.SetPosition(BackButton.mEntityID, ref newPosMainMenu);
    SetPageActive();
  }

  // Update is called once per frame
  void Update()
  { Debug.Log(Input.mousePosition.X + "," + Input.mousePosition.Y);
    if (isTransitioning)
    {
      transitionTimer += Time.deltaTime;
      if (transitionTimer >= transitionDuration)
      {
        InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\mainmenu.scn"); // We finish the transition, time to go back to main menu
      }

    }

    else if (isPageActive && !isTransitioning)
    {

      if (BrightnessSlider.IsClicked)
      {
        HideAllText();
        GammaBorder.SetActive(true);
        GammaText.SetActive(true);
        Vector3 originalResume = InternalCalls.GetPosition(BrightnessSlider.mEntityID);
        float NewXPos = screenToCanvas(Input.mousePosition.X, Input.screenWidth);
        Vector3 newPosResume = new Vector3(NewXPos, originalResume.Y, originalResume.Z);
        newPosResume.X = (newPosResume.X < minX) ? minX : (newPosResume.X > maxX) ? maxX : newPosResume.X;
        InternalCalls.SetPosition(BrightnessSlider.mEntityID, ref newPosResume);
        float fraction = normalize(newPosResume.X, minX, maxX);
        fraction = (fraction > 1) ? 1 : (fraction < 0)? 0:fraction;
        InternalCalls.SetBrightness(fraction);

      }

      if (VolumeSlider.IsClicked)
      {
        HideAllText();
        BGMBorder.SetActive(true);
        BGMText.SetActive(true);
        Vector3 originalResume = InternalCalls.GetPosition(VolumeSlider.mEntityID);
        float NewXPos = screenToCanvas(Input.mousePosition.X, Input.screenWidth);
        Vector3 newPosResume = new Vector3(NewXPos, originalResume.Y, originalResume.Z);
        newPosResume.X = (newPosResume.X < minX) ? minX : (newPosResume.X > maxX) ? maxX : newPosResume.X;
        InternalCalls.SetPosition(VolumeSlider.mEntityID, ref newPosResume);
        float fraction = 1 - normalize(NewXPos, minX, maxX);
        fraction = (fraction > 1) ? 1 : (fraction < 0) ? 0 : fraction;
        InternalCalls.SetSoundBGMVolume(fraction);
      }

      if (SFXSlider.IsClicked)
      {
        HideAllText();
        SFXBorder.SetActive(true);
        SFXText.SetActive(true);
        Vector3 originalResume = InternalCalls.GetPosition(SFXSlider.mEntityID);
        float NewXPos = screenToCanvas(Input.mousePosition.X, Input.screenWidth);
        Vector3 newPosResume = new Vector3(NewXPos, originalResume.Y, originalResume.Z);
        newPosResume.X = (newPosResume.X < minX) ? minX : (newPosResume.X > maxX) ? maxX : newPosResume.X;
        InternalCalls.SetPosition(SFXSlider.mEntityID, ref newPosResume);
        float fraction = 1 - normalize(NewXPos, minX, maxX);
        fraction = (fraction > 1) ? 1 : (fraction < 0) ? 0 : fraction;
        InternalCalls.SetSoundSFXVolume(fraction);
      }

      if (BackButton.IsVisible)
      {
        BackButton.SetActive(true);
        if (BackButton.IsClicked)
        {
          isTransitioning = true;
          transition.StartTransitionInOut(transitionDuration, Transition.TransitionType.FADE);

        }
      }
      else
      {
        BackButton.SetActive(false);
      }

    }

    


  }

  public void SetPageActive()
  {
    isPageActive = true;
    BackButton.SetActive(false);
    isTransitioning = false;
    transitionTimer = 0f;
    Vector3 originalMenu = InternalCalls.GetPosition(mEntityID);
    Vector3 newPosMenu = new Vector3(originalMenu.X, originalMenu.Y, TargetZMenu);
    InternalCalls.SetPosition(mEntityID, ref newPosMenu);

    Vector3 originaVol = InternalCalls.GetPosition(VolumeSlider.mEntityID);
    float volNorm = 1 - InternalCalls.GetSoundBGMVolume();
    float VNewXPos = minX + ((1 - volNorm) * (maxX - minX));
    Vector3 VnewPosSettings = new Vector3(VNewXPos, originaVol.Y, TargetZButton);
    InternalCalls.SetPosition(VolumeSlider.mEntityID, ref VnewPosSettings);

    Vector3 originaSFX = InternalCalls.GetPosition(SFXSlider.mEntityID);
    float SFXNorm = 1 - InternalCalls.GetSoundSFXVolume();
    float SNewXPos = minX + ((1 - SFXNorm) * (maxX - minX));
    Vector3 SnewPosSettings = new Vector3(SNewXPos, originaSFX.Y, TargetZButton);
    InternalCalls.SetPosition(SFXSlider.mEntityID, ref SnewPosSettings);

    Vector3 originalSettings = InternalCalls.GetPosition(BrightnessSlider.mEntityID);
    float gammaNorm = InternalCalls.GetGammaNorm();
    float NewXPos = minX + ((1 - gammaNorm) * (maxX - minX));
    Vector3 newPosSettings = new Vector3(NewXPos, originalSettings.Y, TargetZButton);
    InternalCalls.SetPosition(BrightnessSlider.mEntityID, ref newPosSettings);
    HideAllText();
    Vector3 originalMainMenu = InternalCalls.GetPosition(BackButton.mEntityID);
    Vector3 newPosMainMenu = new Vector3(originalMainMenu.X, originalMainMenu.Y, TargetZButton);
    InternalCalls.SetPosition(BackButton.mEntityID, ref newPosMainMenu);
  }

  private void HideAllText()
  {
    SFXText.SetActive(false);
    BGMText.SetActive(false);
    GammaText.SetActive(false);
    SFXBorder.SetActive(false);
    BGMBorder.SetActive(false);
    GammaBorder.SetActive(false);
  }

  float screenToCanvas(float screenX, float screenMaxX, float screenMinX = 0f)
  {
    // Calculate screen and canvas widths
    float screenWidth = screenMaxX - screenMinX;
    float canvasWidth = CanvasmaxX - CanvasminX;

    //Debug.Log("SX: " + screenX.ToString());
    //Debug.Log("SW: " + screenWidth.ToString());
    //Debug.Log("CW: " + canvasWidth.ToString());

    // Normalize screenX to [0, 1] range, then scale to canvas width
    float normalizedX = (screenX - screenMinX) / screenWidth;
    float canvasX = CanvasminX + (normalizedX * canvasWidth);
    //Debug.Log("NX: " + normalizedX.ToString());
    //Debug.Log("CX: " + canvasX.ToString());


    return canvasX;
  }

  float normalize(float value, float min, float max)
  {
    return 1 - ((value - min) / (max - min));
  }
}



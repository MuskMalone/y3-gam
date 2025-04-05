using IGE.Utils;
using System;
using System.Collections.Generic;
using System.IO.Ports;
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

    public Entity WASDtoRotate;
    public Entity ExitToEscape;

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
    UV_LIGHT,
  }
  private enum LightState
  {
    UV_LIGHT_NOT_MOVING,
    UV_LIGHT_START_MOVE,
    UV_LIGHT_MOVING,
    UV_LIGHT_STOP_MOVE
  }
  private State currState = State.DEFAULT;
  private LightState currentLightState = LightState.UV_LIGHT_NOT_MOVING;
  private bool areLightsOn = true;
  private bool defaultStateActive = true;
  private Vector3 previousRotationVelo = new Vector3(0, 0, 0);
  private Vector3 currentRotationVelo = new Vector3(0, 0, 0);

  private float currShakesize = 5f;
  public float Shakesize = 5f;
  public float shakeTime = 1f;
  private float currShakeTime;
  public float humVolume = 1f;

  private Vector3 rotationBeforeShake = new Vector3();

  public ControlPanel2() : base() { }

  void Start()
  {
    if (playerMove == null)
    {
      Debug.LogError("[ControlPanel2.cs] PlayerMove Script Entity not found!");
      return;
    }

    InitHiddenTexts();

    WASDtoRotate.SetActive(false);
        ExitToEscape.SetActive(false);
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
          SetControlPanelCameraAsMain();
          playerMove.FreezePlayer();
                    WASDtoRotate.SetActive(true);
                    ExitToEscape.SetActive(true);
                    if (!areLightsOn)
          {
            SetHiddenText(currStatue, true);
            UVLight.SetActive(true);
            uvLightEnabled = true;
            InternalCalls.PlaySound(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Startup_SFX.wav");
            InternalCalls.PlaySound(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Humming_SFX.wav");

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
                    WASDtoRotate.SetActive(false);
                    ExitToEscape.SetActive(false);
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
          InternalCalls.StopSound(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Humming_SFX.wav");
                    WASDtoRotate.SetActive(false);
                    ExitToEscape.SetActive(false);
                    currState = State.OPEN;
        }
        break;


      case State.CLOSED:
        break;
      default:
        break;
    }

    UpdateHiddenTextVisibility();


  }
  private Vector3 AddRandomDirection(Vector3 originalRotation, float currShakeSize)
  {
    // Generate random angles for the direction vector.
    double theta = 2 * Math.PI * new Random().NextDouble(); // Random angle between 0 and 2*pi
    double phi = Math.Acos(2 * new Random().NextDouble() - 1); // Random angle between 0 and pi

    // Convert spherical coordinates to Cartesian coordinates.
    float x = currShakeSize * (float)(Math.Sin(phi) * Math.Cos(theta));
    float y = currShakeSize * (float)(Math.Sin(phi) * Math.Sin(theta));
    float z = currShakeSize * (float)Math.Cos(phi);

    // Create the random direction vector.
    Vector3 randomDirection = new Vector3(x, y, z);

    // Add the random direction to the original rotation.
    return originalRotation + randomDirection;
  }
  private void HandleUVLightControls()
  {
    Vector3 currentRotation = InternalCalls.GetRotationEuler(UVLight.mEntityID);
    float deltaTime = Time.deltaTime;
    bool playerInput = false;
    previousRotationVelo = currentRotationVelo;
    currentRotationVelo = new Vector3();
    if (Input.GetKeyHeld(KeyCode.W))
    {
      currentRotationVelo.X += rotationSpeed * deltaTime;
      playerInput = true;
    }
    if (Input.GetKeyHeld(KeyCode.S))
    {
      currentRotationVelo.X -= rotationSpeed * deltaTime;
      playerInput = true;
    }
    if (Input.GetKeyHeld(KeyCode.A))
    {
      currentRotationVelo.Y += rotationSpeed * deltaTime;
      playerInput = true;
    }
    if (Input.GetKeyHeld(KeyCode.D))
    {
      currentRotationVelo.Y -= rotationSpeed * deltaTime;
      playerInput = true;
    }
    if (previousRotationVelo.LengthSquared() <= float.Epsilon && currentRotationVelo.LengthSquared() > 0)
    {
      //just entered moving state
      InternalCalls.PlaySound(UVLight.mEntityID, "..\\Assets\\Audio\\UV_StartMoving_SFX.wav");
      InternalCalls.SetSoundVolume(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Move_SFX.wav", humVolume);
      rotationBeforeShake = currentRotation;
      currShakeTime = shakeTime;
      
      currentLightState = LightState.UV_LIGHT_START_MOVE;
    }
    else if (previousRotationVelo.LengthSquared() > 0 && currentRotationVelo.LengthSquared() <= float.Epsilon) {
      //just exited moving state
      InternalCalls.PlaySound(UVLight.mEntityID, "..\\Assets\\Audio\\UV_StopMoving_SFX.wav");

      rotationBeforeShake = currentRotation;
      currShakeTime = shakeTime;
      currentLightState = LightState.UV_LIGHT_STOP_MOVE;
    }

    switch (currentLightState) {
      case LightState.UV_LIGHT_START_MOVE:
        currShakeTime -= InternalCalls.GetDeltaTime();
        currentRotation = AddRandomDirection(rotationBeforeShake, currShakesize);
        currShakesize = Easing.Linear(Shakesize, 0, 1 - (currShakeTime/shakeTime)); // decay the shake from shakesize to 0
        InternalCalls.SetSoundPitch(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Move_SFX.wav", Easing.Linear(0, 1, 1 - (currShakeTime / shakeTime)));
        Debug.Log($"{currShakesize} rotationBeforeShake{rotationBeforeShake} currRot{currentRotation}");
        if (currShakeTime <= 0) // times up 
        {
          currentRotation = rotationBeforeShake;
          currentLightState = LightState.UV_LIGHT_MOVING;
        }
        break;
      case LightState.UV_LIGHT_MOVING:
        currentRotation += currentRotationVelo;
        break;
      case LightState.UV_LIGHT_STOP_MOVE:
        currShakeTime -= InternalCalls.GetDeltaTime();
        currentRotation = AddRandomDirection(rotationBeforeShake, currShakesize);
        currShakesize = Easing.Linear(Shakesize, 0, 1 - (currShakeTime / shakeTime)); // decay the shake from shakesize to 0
        InternalCalls.SetSoundPitch(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Move_SFX.wav", Easing.Linear(1, 0, 1 - (currShakeTime / shakeTime)));
        playerInput = true;
        if (currShakeTime <= 0) // times up 
        {
          currentRotation = rotationBeforeShake;
          currentLightState = LightState.UV_LIGHT_NOT_MOVING;
          InternalCalls.SetSoundVolume(UVLight.mEntityID, "..\\Assets\\Audio\\UV_Move_SFX.wav", 0);

        }
        break;
      case LightState.UV_LIGHT_NOT_MOVING:
        break;
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
      //uvLightEnabled = false; // Optionally reset this flag if it’s used only when lights are off
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
      //odysseusStatue.SetActive(false);
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
    odysseusStatue.SetActive(false);
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

  private void UpdateHiddenTextVisibility()
  {
    if (!areLightsOn && uvLightEnabled)
    {
      if (defaultStateActive)
        SetDefaultHiddenText(true);
      else
        SetHiddenText(currStatue, true);
    }
    else
    {
      SetDefaultHiddenText(false);
      SetHiddenText(currStatue, false);
    }
  }

}
using IGE.Utils;
using System.Numerics;

public class ControlPanel : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity UVLight;
  public Entity[] hiddenText;
  public PlayerMove playerMove;
  public float rotationSpeed = 50.0f; // degrees per second

  public float minVerticalRotation = -17.0f; // Minimum pitch
  public float maxVerticalRotation = 25.0f;  // Maximum pitch
  public float minHorizontalRotation = -36.0f; // Minimum yaw
  public float maxHorizontalRotation = 32.0f;  // Maximum yaw

  private bool controllingLights = false;

  public ControlPanel() : base()
  {

  }

  void Start()
  {
    if (playerMove == null)
    {
      Debug.LogError("[ControlPanel.cs] PlayerMove Script Entity not found!");
      return;
    }

    foreach (Entity text in hiddenText)
    {
      text?.SetActive(false);
    }
  }

  void Update()
  {
    bool mouseClicked = Input.GetMouseButtonTriggered(0);
    bool isPanelHit = playerInteraction.RayHitString == "Control Panel";

    if (mouseClicked && isPanelHit)
    {
      controllingLights = true;
      foreach (Entity text in hiddenText)
      {
        text?.SetActive(true);
      }
    }

    if (controllingLights)
    {
      playerMove.FreezePlayer();
      Vector3 currentRotation = InternalCalls.GetRotationEuler(UVLight.mEntityID);
      float deltaTime = Time.deltaTime;

      if (Input.GetKeyDown(KeyCode.W))
      {
        currentRotation.X += rotationSpeed * deltaTime;
      }

      if (Input.GetKeyDown(KeyCode.S))
      {
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
        foreach (Entity text in hiddenText)
        {
          text?.SetActive(false);
        }
      }
    }

    UVLight.SetActive(controllingLights);
  }
}
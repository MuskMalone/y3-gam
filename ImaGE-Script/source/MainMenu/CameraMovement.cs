using IGE.Utils;
using System;
using System.Numerics;

public class CameraMovement : Entity
{
  public Entity cam;
  public bool canLook = true;
  public float sensitivity = 0.05f;
  public float maxPitch = 89f;
  public float minPitch = -89f;

  private float yaw = 0f;
  private float pitch = 0f;
  private bool skipNextMouseDelta = false;
  private PanOutFromTV panOutScript;
  private bool startPanning = false;

  void Start()
  {
    panOutScript = FindScript<PanOutFromTV>();
  }

  void Init()
  {
    // Use the initial rotation instead
    pitch = InternalCalls.GetRotationEuler(mEntityID).X;
    yaw = InternalCalls.GetRotationEuler(mEntityID).Y;

    InternalCalls.ShowCursor();
  }

  void Update()
  {
    if (!startPanning)
    {
      if (!panOutScript.HasInitialSequenceEnded()) { return; }

      Init();
      startPanning = true;
    }

    ProcessLook();
  }

  void ProcessLook()
  {
    if (canLook)
    {
      Vector3 mouseDelt = InternalCalls.GetMouseDelta();

      if (skipNextMouseDelta && mouseDelt.X != 0.0f && mouseDelt.Y != 0.0f)
      {
        skipNextMouseDelta = false;
        return;
      }

      float mouseDeltaX = mouseDelt.X;
      float mouseDeltaY = mouseDelt.Y;

      yaw -= mouseDeltaX * sensitivity;
      pitch -= mouseDeltaY * sensitivity;
    }

    if (yaw > 360.0f || yaw < -360.0f)
      yaw /= 360.0f;

    pitch = Mathf.Clamp(pitch, minPitch, maxPitch);
    cam.GetComponent<Transform>().rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitX, Mathf.DegToRad(pitch));

    GetComponent<Transform>().rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, Mathf.DegToRad(yaw));
    InternalCalls.UpdatePhysicsToTransform(mEntityID);
  }

  public Vector2 GetRotation()
  {
    return new Vector2(pitch, yaw);
  }

  public void SetRotation(Vector3 rot)
  {
    yaw = rot.Y;
    pitch = rot.X;
  }
}
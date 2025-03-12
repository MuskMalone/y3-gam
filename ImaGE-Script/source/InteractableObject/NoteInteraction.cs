using IGE.Utils;
using System;
using System.Numerics;

public class NoteInteraction : Entity
{

  public PlayerMove playerMove;
  public Entity noteUI;
  public PlayerInteraction playerInteraction;
  public Entity viewNoteUI;
  public Entity playerCamera;
  public float finalDistanceAwayFromCam = 2f;

  public float moveSpeed = 5f;
  public float rotateSpeed = 5f;

  private bool isMoving = false;
  private bool isReset = false;
  private Vector3 originalPos;
  private Quaternion originalRot;
  private string noteTag;
  private bool noteInteractedWith = false;

  public NoteInteraction() : base()
  {

  }

  void Start()
  {
    if (noteUI == null)
    {
      Debug.LogError("[NoteInteraction.cs] Note UI Entity not found!");
      return;
    }

    noteUI?.SetActive(false);
    viewNoteUI?.SetActive(false);

    if (playerMove == null)
    {
      Debug.LogError("[NoteInteraction.cs] PlayerMove Script Entity not found!");
      return;
    }

    originalPos = InternalCalls.GetWorldPosition(mEntityID);
    originalRot = InternalCalls.GetWorldRotation(mEntityID);

    noteTag = InternalCalls.GetTag(mEntityID);
  }

  void Update()
  {
    bool mouseClicked = Input.GetMouseButtonTriggered(0);
    bool isNoteHit = playerInteraction.RayHitString == noteTag;

    bool noteIsActive = noteUI.IsActive();

    if (mouseClicked && !isMoving && !isReset)
    {
      if (isNoteHit && !noteIsActive)
      {
        isMoving = true;
        noteInteractedWith = true;
        viewNoteUI.SetActive(false);
      }
      else if (noteIsActive)
      {
        HideNoteUI();
        isReset = true;
      }
    }

    if (!noteInteractedWith)
      viewNoteUI.SetActive(isNoteHit);

    if (isMoving)
    {
      MoveToTarget();
      playerMove.FreezePlayer();
    }

    if (isReset)
    {
      Reset();
    }
  }

  private void ShowNoteUI()
  {
    InternalCalls.PlaySound(mEntityID, "ViewNote");
    if (noteUI != null && playerMove != null)
    {
      noteUI.SetActive(true);
      playerMove.FreezePlayer();
      viewNoteUI.SetActive(false);
    }
  }

  private void HideNoteUI()
  {
    InternalCalls.PlaySound(mEntityID, "ViewNote");
    if (noteUI != null && playerMove != null)
    {
      noteUI.SetActive(false);
    }
  }

  private void MoveToTarget()
  {
    // Current Position
    Vector3 currPos = InternalCalls.GetWorldPosition(mEntityID);

    // Current Rotation
    Quaternion currRot = InternalCalls.GetWorldRotation(mEntityID);

    // Target Position (Adjust forward offset to place in front of the camera)    
    Vector3 targetPos = InternalCalls.GetWorldPosition(playerCamera.mEntityID) + InternalCalls.GetCameraForward() * finalDistanceAwayFromCam;

    // Calculate the direction from the note to the camera
    Vector3 directionToCamera = Vector3.Normalize(InternalCalls.GetWorldPosition(playerCamera.mEntityID) - targetPos);

    // Compute Target Rotation
    Vector3 up = Vector3.Normalize(Vector3.Cross(InternalCalls.GetCameraForward(), InternalCalls.GetCameraRight()));
    Quaternion targetRot = CalculateLookRotation(-directionToCamera, up);

    //Quaternion additionalRotation = AxisAngleToQuaternion(new Vector3(1,0,0), 90);
    Quaternion additionalRotation = new Quaternion(-0.7071f, 0f, 0f, 0.7071f); // I am hard coding this for now, it's -90deg in quat
    Debug.Log("Additional Rotation: " + additionalRotation);
    float debugAngle = 2 * Mathf.Acos(additionalRotation.W) * Mathf.Rad2Deg;
    Debug.Log("Additional Rotation Angle: " + debugAngle + " degrees");
    targetRot = targetRot * additionalRotation;
    Debug.Log("Final Target Rotation: " + targetRot);

    // Move towards the target position
    Vector3 newPos = Vector3.Lerp(currPos, targetPos, Time.deltaTime * moveSpeed);
    InternalCalls.SetWorldPosition(mEntityID, ref newPos);

    // Rotate towards the corrected target rotation
    Quaternion newRot = Quaternion.Slerp(currRot, targetRot, Time.deltaTime * rotateSpeed);
    InternalCalls.SetWorldRotation(mEntityID, ref newRot);

    InternalCalls.UpdatePhysicsToTransform(mEntityID);

    // Stop moving when close enough
    if (Vector3.Distance(newPos, targetPos) < 0.1f)
    {
      Debug.Log("Finished");
      InternalCalls.SetWorldPosition(mEntityID, ref targetPos);
      InternalCalls.SetWorldRotation(mEntityID, ref targetRot);
      InternalCalls.UpdatePhysicsToTransform(mEntityID);
      isMoving = false;
      ShowNoteUI();
    }
  }

  private void Reset()
  {
    // Current Position
    Vector3 currPos = InternalCalls.GetWorldPosition(mEntityID);

    // Current Rotation
    Quaternion currRot = InternalCalls.GetWorldRotation(mEntityID);

    // Target Position 
    Vector3 targetPos = originalPos;

    // Compute Target Rotation (Make the note face the camera)
    Quaternion targetRot = originalRot;

    // Move towards the target position
    Vector3 newPos = Vector3.Lerp(currPos, targetPos, Time.deltaTime * moveSpeed);
    InternalCalls.SetWorldPosition(mEntityID, ref newPos);

    // Rotate towards the corrected target rotation
    Quaternion newRot = Quaternion.Slerp(currRot, targetRot, Time.deltaTime * rotateSpeed);
    InternalCalls.SetWorldRotation(mEntityID, ref newRot);

    InternalCalls.UpdatePhysicsToTransform(mEntityID);

    // Stop moving when close enough
    if (Vector3.Distance(newPos, targetPos) < 0.1f)
    {
      Debug.Log("Finished");
      InternalCalls.SetWorldPosition(mEntityID, ref targetPos);
      InternalCalls.SetWorldRotation(mEntityID, ref targetRot);
      InternalCalls.UpdatePhysicsToTransform(mEntityID);
      playerMove.UnfreezePlayer();
      noteInteractedWith = false;
      isReset = false;
    }
  }

  // This function is not working
  private Quaternion AxisAngleToQuaternion(Vector3 axis, float angleInDegrees)
  {
    float angleInRadians = angleInDegrees * Mathf.Deg2Rad;
    float halfAngle = angleInRadians * 0.5f;
    float sinHalfAngle = Mathf.Sin(halfAngle);
    float cosHalfAngle = Mathf.Cos(halfAngle);

    // Ensure the axis is normalized
    axis = Vector3.Normalize(axis);

    return new Quaternion(
        axis.X * sinHalfAngle,
        axis.Y * sinHalfAngle,
        axis.Z * sinHalfAngle,
        cosHalfAngle
    );
  }

  private Quaternion CalculateLookRotation(Vector3 forward, Vector3 up)
  {
    // Normalize the forward vector
    forward = Vector3.Normalize(forward);

    // Calculate the right vector
    Vector3 right = Vector3.Normalize(Vector3.Cross(up, forward));

    // Recalculate the up vector to ensure orthogonality
    Vector3 newUp = Vector3.Normalize(Vector3.Cross(forward, right));

    // Create a rotation matrix from the forward, right, and up vectors
    Matrix4x4 rotationMatrix = new Matrix4x4(
        right.X, right.Y, right.Z, 0,
        newUp.X, newUp.Y, newUp.Z, 0,
        forward.X, forward.Y, forward.Z, 0,
        0, 0, 0, 1
    );
    
    // Convert the rotation matrix to a quaternion
    return Quaternion.CreateFromRotationMatrix(rotationMatrix);
  }
}
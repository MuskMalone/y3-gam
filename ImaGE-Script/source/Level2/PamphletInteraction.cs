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

public class PamphletInteraction : Entity
{
  public Entity Page1Pivot, Page2Pivot, Page3Pivot;
  public Vector3 page1AmtToRotate, page3AmtToRotate;
  public float animationTime;

  public PlayerMove playerMove;
  public Entity noteUI;
  public PlayerInteraction playerInteraction;
  public Entity viewNoteUI;
  public Entity playerCamera;
  public float finalDistanceAwayFromCam = 2f;

  public float moveSpeed = 5f;
  public float rotateSpeed = 5f;

  private bool isOpening = true;  // false means closing
  private Vector3 originalPos;
  private Quaternion originalRot;
  private string noteTag;
  private bool noteInteractedWith = false;

  private float timeElapsed = 0f;
  private enum State
  {
    IDLE,
    FLIPPING,
    MOVING,
    OPENED
  }
  private State currState = State.IDLE;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      case State.IDLE:
        {
          bool isNoteHit = playerInteraction.RayHitString == noteTag;

          if (Input.GetMouseButtonTriggered(0))
          {
            bool noteIsActive = noteUI.IsActive();
            
            if (noteIsActive && isNoteHit)
            {
              isOpening = true;
              noteInteractedWith = true;
              viewNoteUI.SetActive(false);
            }
            else if (noteIsActive)
            {
              HideNoteUI();
              isOpening = false;
            }
            currState = State.MOVING;
          }

          if (!noteInteractedWith)
          {
            viewNoteUI.SetActive(isNoteHit);
          }

          break;
        }

      case State.MOVING:
        {
          if (isOpening)
          {
            MoveToTarget();
            playerMove.FreezePlayer();
          }
          else
          {
            MoveToOriginal();
          }

          break;
        }

      case State.FLIPPING:
        {
          timeElapsed += InternalCalls.GetDeltaTime();

          if (timeElapsed >= animationTime)
          {
            timeElapsed = 0f;
            currState = State.IDLE;
            ShowNoteUI();
            Debug.Log("Changing to " + (isOpening ? "Closing" : "Opening"));
            isOpening = !isOpening;
            return;
          }

          float t = Mathf.SmoothStep(timeElapsed / animationTime);
          if (!isOpening) { t = 1 - t; }
          FlipPamphlet(t);

          break;
        }
      default:
        return;
    }
  }

  void FlipPamphlet(float t)
  {
    Vector3 newRot1 = t * page1AmtToRotate, newRot2 = t * page3AmtToRotate;
    InternalCalls.SetRotationEuler(Page1Pivot.mEntityID, ref newRot1);
    InternalCalls.SetRotationEuler(Page3Pivot.mEntityID, ref newRot2);
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
      currState = State.FLIPPING;
    }
  }

  private void MoveToOriginal()
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
      currState = State.FLIPPING;
    }
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

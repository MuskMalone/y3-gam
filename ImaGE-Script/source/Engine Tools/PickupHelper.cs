using IGE.Utils;
using System;
using System.Numerics;

public static class Pickup
{
  public static bool MoveAndShrink(Entity entity, uint playerID, uint playerCameraID, float finalDistanceAwayFromCamera = 3f)
  {
    //Vector3 smallScale = new Vector3(0f, 0f, 0f);
    //InternalCalls.SetBoxColliderScale(entity.mEntityID, smallScale);
    InternalCalls.RemoveBoxCollider(entity.mEntityID);

    // Hardcoded values
    float moveSpeed = 5f;
    float rotateSpeed = 2f;
    float scaleSpeed = 0f;

    // Current Transform Properties
    Vector3 currPos = InternalCalls.GetWorldPosition(entity.mEntityID);
    Quaternion currRot = InternalCalls.GetWorldRotation(entity.mEntityID);
    Vector3 currScale = InternalCalls.GetScale(entity.mEntityID);

    // Target Transform Properties
    Vector3 targetPos = InternalCalls.GetWorldPosition(playerCameraID) + InternalCalls.GetCameraForward() * finalDistanceAwayFromCamera;
    Vector3 directionToCamera = Vector3.Normalize(InternalCalls.GetWorldPosition(playerCameraID) - targetPos);
    Vector3 up = Vector3.Normalize(Vector3.Cross(InternalCalls.GetCameraForward(), InternalCalls.GetCameraRight()));
    Quaternion targetRot = Pickup.CalculateLookRotation(directionToCamera, up);
    Quaternion additionalRotation = new Quaternion(0f, 0f, 1f, 0f);
    targetRot = targetRot * additionalRotation;
    Vector3 targetScale = new Vector3(0, 0, 0);

    // Lerp towards target
    Vector3 newPos = Vector3.Lerp(currPos, targetPos, Time.deltaTime * moveSpeed);
    Quaternion newRot = Quaternion.Slerp(currRot, targetRot, Time.deltaTime * rotateSpeed);
    Vector3 newScale = Vector3.Lerp(currScale, targetScale, Time.deltaTime * scaleSpeed);

    InternalCalls.SetWorldPosition(entity.mEntityID, ref newPos);
    InternalCalls.SetWorldRotation(entity.mEntityID, ref newRot);
    InternalCalls.SetScale(entity.mEntityID, ref newScale);
    InternalCalls.UpdatePhysicsToTransform(entity.mEntityID);

    if (Vector3.Distance(newPos, targetPos) < 0.1f)
    {
      InternalCalls.SetWorldPosition(entity.mEntityID, ref targetPos);
      InternalCalls.SetWorldRotation(entity.mEntityID, ref targetRot);
      InternalCalls.UpdatePhysicsToTransform(entity.mEntityID);
      return true; // End, time to pickup
    }

    return false; // Continue lerping
  }

  private static Quaternion CalculateLookRotation(Vector3 forward, Vector3 up)
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

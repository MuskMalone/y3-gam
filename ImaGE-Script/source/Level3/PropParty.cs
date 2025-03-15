using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class PropParty : Entity
{
  public Entity[] partyEntities;
  public float updateTimeInterval = 1f;
  public float rangeOfMovement = 2f;
  public float speedOfMovement = 0.6f;
  public float speedOfRotation = 0.6f;

  private Vector3[] initialPositions;
  private Quaternion[] initialRotations;
  private Vector3[] targetPositions;
  private Quaternion[] targetRotations;
  private float[] timeToNextUpdate;
  private Random random;

  public PropParty() : base()
  {
    random = new Random();
  }

  void Start()
  {
    initialPositions = new Vector3[partyEntities.Length];
    initialRotations = new Quaternion[partyEntities.Length];
    targetPositions = new Vector3[partyEntities.Length];
    targetRotations = new Quaternion[partyEntities.Length];
    timeToNextUpdate = new float[partyEntities.Length];

    for (int i = 0; i < partyEntities.Length; i++)
    {
      initialPositions[i] = InternalCalls.GetWorldPosition(partyEntities[i].mEntityID);
      initialRotations[i] = InternalCalls.GetWorldRotation(partyEntities[i].mEntityID);
      targetPositions[i] = initialPositions[i];
      targetRotations[i] = initialRotations[i];
      timeToNextUpdate[i] = updateTimeInterval;
    }
  }

  void Update()
  {
    for (int i = 0; i < partyEntities.Length; i++)
    {
      // Decrease the time to next random update
      timeToNextUpdate[i] -= Time.deltaTime;

      // When it's time to generate a new random position and rotation
      if (timeToNextUpdate[i] <= 0f)
      {
        // Generate a random position offset and scale it to your desired movement range
        Vector3 randomPositionOffset = new Vector3(
            (float)(random.NextDouble() * 2 - 1), // Random X between -1 and 1
            (float)(random.NextDouble() * 2 - 1), // Random Y between -1 and 1
            (float)(random.NextDouble() * 2 - 1)  // Random Z between -1 and 1
        ) * rangeOfMovement;

        // Set the target position and rotation
        targetPositions[i] = initialPositions[i] + randomPositionOffset;

        Vector3 randomAxis = new Vector3(
            (float)(random.NextDouble() * 2 - 1), // Random X component of the axis
            (float)(random.NextDouble() * 2 - 1), // Random Y component of the axis
            (float)(random.NextDouble() * 2 - 1)  // Random Z component of the axis
        );
        randomAxis = Vector3.Normalize(randomAxis); // Normalize to make it a unit vector

        // Random rotation angle between 0 and 2π radians
        float randomRotationAngle = (float)(random.NextDouble() * Math.PI * 2);

        // Create the quaternion representing the random rotation
        Quaternion randomRotation = Quaternion.CreateFromAxisAngle(randomAxis, randomRotationAngle);

        // Set the target rotation
        targetRotations[i] = Quaternion.Concatenate(initialRotations[i], randomRotation);

        // Reset the timer for the next update
        timeToNextUpdate[i] = updateTimeInterval;
      }

      Vector3 newPos = Vector3.Lerp(
          InternalCalls.GetWorldPosition(partyEntities[i].mEntityID),
          targetPositions[i],
          Time.deltaTime * speedOfMovement
      );

      // Set the updated position
      InternalCalls.SetWorldPosition(partyEntities[i].mEntityID, ref newPos);
      InternalCalls.UpdatePhysicsToTransform(partyEntities[i].mEntityID);

      //Debug.Log(InternalCalls.GetWorldPosition(partyEntities[i].mEntityID).ToString());

      Quaternion newRot = Quaternion.Slerp(
          InternalCalls.GetWorldRotation(partyEntities[i].mEntityID),
          targetRotations[i],
          Time.deltaTime * speedOfRotation
      );

      // Set the updated rotation
      InternalCalls.SetWorldRotation(partyEntities[i].mEntityID, ref newRot);
      InternalCalls.UpdatePhysicsToTransform(partyEntities[i].mEntityID);
    }
  }
}
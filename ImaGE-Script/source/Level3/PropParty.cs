using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class PropParty : Entity
{
  public Entity[] partyEntities;

  private Vector3[] initialPositions;
  private Quaternion[] initialRotations;
  private Random random;

  public PropParty() : base()
  {
    random = new Random();
  }

  void Start()
  {
    initialPositions = new Vector3[partyEntities.Length];
    initialRotations = new Quaternion[partyEntities.Length];

    for (int i = 0; i < partyEntities.Length; i++)
    {
      initialPositions[i] = InternalCalls.GetWorldPosition(partyEntities[i].mEntityID);
      initialRotations[i] = InternalCalls.GetWorldRotation(partyEntities[i].mEntityID);
    }
  }

  void Update()
  {
    for (int i = 0; i < partyEntities.Length; i++)
    {
      Vector3 randomPositionOffset = new Vector3(
          (float)(random.NextDouble() * 2 - 1), // Random X between -1 and 1
          (float)(random.NextDouble() * 2 - 1), // Random Y between -1 and 1
          (float)(random.NextDouble() * 2 - 1)  // Random Z between -1 and 1
      ) * 5f;

      // Update position by adding the random offset to the initial position
      Vector3 newPos = initialPositions[i] + randomPositionOffset;
      InternalCalls.SetWorldPosition(partyEntities[i].mEntityID, ref newPos);

      // Rotate the entity randomly around the Y axis
      float randomRotationAngle = (float)(random.NextDouble() * Math.PI * 2); // Random angle in radians
      Quaternion randomRotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, randomRotationAngle);

      // Apply the rotation to the entity
      Quaternion newRot = Quaternion.Concatenate(initialRotations[i], randomRotation);
      InternalCalls.SetWorldRotation(partyEntities[i].mEntityID, ref newRot);
    }
  }
}


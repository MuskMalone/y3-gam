using System;
using System.Numerics;
using IGE.Utils;

public class Ladder : Entity
{
  public PlayerMove playerMove;
  public PlayerInteraction playerInteraction;
  public bool isOnLadder = false;
  public float ladderSpeed = 30f; // Adjust speed as needed
  public float maxLadderHeight = 55f; // Max Y height player can climb
  public float minLadderHeight = 0f;  // Min Y height (bottom of the ladder)
  public Entity ClickUI;  // This UI will show when hovering over the ladder or at the top/bottom
  public bool isLevel2 = false;
  public Transition transition;

  public float inFrontOfLadderOffset = 2f;
  private bool triggerQuickTransition = false;

  public Ladder() : base()
  {
  }

  void Start()
  {
    if (playerMove == null)
    {
      Debug.LogError("[Ladder.cs] PlayerMove Script Entity not found!");
      return;
    }
    // Initially hide the ClickUI.
    ClickUI?.SetActive(false);
  }

  void Update()
  {
    bool mouseClicked = Input.GetMouseButtonTriggered(0);
    bool isLadderHit = playerInteraction.RayHitString == "Ladder";
    Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

    // When not on the ladder, show the ClickUI if the player's ray is hitting the ladder.
    if (!isOnLadder)
    {
      ClickUI?.SetActive(isLadderHit);
    }
    else
    {
      // When on the ladder, show the ClickUI if at the top or bottom.
      if (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight)
        ClickUI?.SetActive(true);
      else
        ClickUI?.SetActive(false);
    }

    // Toggle ladder state:
    // - If NOT on the ladder, require the player to be looking at the ladder (isLadderHit).
    // - If already on the ladder, allow toggling if at the top or bottom (max or min height).
    if (mouseClicked)
    {
      if (!isOnLadder && isLadderHit)
      {
        playerMove.FreezePlayer();
        transition.StartTransition(true, 0.5f, Transition.TransitionType.FADE);
        triggerQuickTransition = true;
      }
      else if (isOnLadder && (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight))
      {
        ToggleLadderState();
      }
    }

    if (triggerQuickTransition && transition.IsFinished())
    {
      transition.StartTransition(false, 0.5f, Transition.TransitionType.FADE);

      // Move the player into the optimal position for climbing
      MoveToOptimalPosition(playerPosition);
    
      ToggleLadderState();
      triggerQuickTransition = false;
    }

    if (isOnLadder)
    {
      HandleLadderMovement();
    }
  }

  private void ToggleLadderState()
  {
    // Toggle the ladder state.
    isOnLadder = !isOnLadder;
    playerMove.climbing = isOnLadder;
    Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

    if (isOnLadder)
    {
      // When starting to climb, hide the ClickUI and disable normal movement.
      ClickUI?.SetActive(false);
      playerMove.FreezePlayer();
      playerMove.canLook = true;
      InternalCalls.SetGravityFactor(playerMove.mEntityID, 0f);
      if (isLevel2)
      {
        Vector3 position = InternalCalls.GetWorldPosition(mEntityID);
        Quaternion rotation = InternalCalls.GetRotation(mEntityID);
        Vector3 halfSize = InternalCalls.GetScale(mEntityID);
        Vector3 localBottomNearLeft = new Vector3(-halfSize.X, -halfSize.Y, -halfSize.Z);
        Vector3 localTopNearLeft = new Vector3(-halfSize.X, halfSize.Y, -halfSize.Z);


        // Transform these local points into world space.
        Vector3 bottomNearLeft = position + Vector3.Transform(localBottomNearLeft, rotation);
        Vector3 topNearLeft = position + Vector3.Transform(localTopNearLeft, rotation);
        if (Vector3.DistanceSquared(playerPosition, topNearLeft) < Vector3.DistanceSquared(playerPosition, bottomNearLeft))
        {
          playerPosition = new Vector3(15.9f, 98f, -477);
          Debug.Log($"{Vector3.DistanceSquared(playerPosition, topNearLeft)} {Vector3.DistanceSquared(playerPosition, bottomNearLeft)} ,{playerPosition}ehhhhhhhhhhhhhhhhhhhhhhhhhhhhw");

          InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
          InternalCalls.UpdatePhysicsToTransform(playerMove.mEntityID);
        }
      }
    }
    else
    {

      // When leaving the ladder, if at the top, offset the player so they land on the platform.
      if (playerPosition.Y >= maxLadderHeight)
      {
        // Check what the player is looking at.
        // If the player's raycast is hitting a wall or ladder, launch them backwards.
        Vector3 direction;
        if (!isLevel2)
        {
          if (playerInteraction.RayHitString == "Wall" || playerInteraction.RayHitString == "Ladder")
          {
            direction = -playerMove.GetComponent<Transform>().forward;
          }
          else
          {
            direction = playerMove.GetComponent<Transform>().forward;
          }
          Vector3 offset = direction * 8.0f; // Adjust offset if needed
          playerPosition += offset;
          InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
        }
      }

      playerMove.UnfreezePlayer();
      InternalCalls.SetGravityFactor(playerMove.mEntityID, playerMove.GetInitialGravityFactor());
      // Hide the ClickUI when not on the ladder.
      ClickUI?.SetActive(false);
    }
  }

  private void HandleLadderMovement()
  {
    float verticalInput = Input.GetAxis("Vertical"); // W (1) for up, S (-1) for down
    Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

    if (Math.Abs(verticalInput) > float.Epsilon)
    {

      Vector3 position = InternalCalls.GetWorldPosition(mEntityID);
      Quaternion rotation = InternalCalls.GetRotation(mEntityID);
      Vector3 halfSize = InternalCalls.GetScale(mEntityID);
      Vector3 localBottomNearLeft = new Vector3(-halfSize.X, -halfSize.Y, -halfSize.Z);
      Vector3 localTopNearLeft = new Vector3(-halfSize.X, halfSize.Y, -halfSize.Z);
      

      // Transform these local points into world space.
      Vector3 bottomNearLeft = position + Vector3.Transform(localBottomNearLeft, rotation);
      Vector3 topNearLeft = position + Vector3.Transform(localTopNearLeft, rotation);
      Debug.Log($"{topNearLeft - bottomNearLeft}, playerPosition {playerPosition}, position {position}, rotation {rotation}, sz {halfSize}, topleft {topNearLeft} ");
      // Prevent climbing above max height.
      if (verticalInput > 0 && playerPosition.Y >= maxLadderHeight)
        return;
      if (verticalInput < 0 && playerPosition.Y <= minLadderHeight)
        return;
      Vector3 velocity = Vector3.Normalize(topNearLeft - bottomNearLeft);
      Debug.Log($"im walking up ladder {velocity * ladderSpeed * InternalCalls.GetDeltaTime() * new Vector3(verticalInput, verticalInput, verticalInput)}");

      // Only modify the Y position.
      playerPosition += velocity * ladderSpeed * InternalCalls.GetDeltaTime() * new Vector3(verticalInput, verticalInput, verticalInput);

      if (playerPosition.Y > maxLadderHeight)
        playerPosition.Y = maxLadderHeight;

      InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

      // Optionally apply a slight downward force to stabilize movement.
      //InternalCalls.MoveCharacter(playerMove.mEntityID, -velocity * 0.1f);
      InternalCalls.UpdatePhysicsToTransform(playerMove.mEntityID);
    }
  }

  private void MoveToOptimalPosition(Vector3 playerPosition)
  {
    Vector3 ladderStart = InternalCalls.GetWorldPosition(mEntityID);
    Vector3 ladderForward = GetForwardVector(mEntityID);
    Vector3 offset = ladderForward * inFrontOfLadderOffset;
    playerPosition.X = ladderStart.X + offset.X;
    playerPosition.Z = ladderStart.Z + offset.Z;
    InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
    InternalCalls.UpdatePhysicsToTransform(playerMove.mEntityID);
    playerMove.UnfreezePlayer();
  }

  private Vector3 GetForwardVector(uint entityID)
  {
    Quaternion rotation = InternalCalls.GetWorldRotation(entityID);
    Vector3 defaultForward = new Vector3(1, 0, 0);
    return Vector3.Normalize(QuaternionMultiply(rotation, defaultForward));
  }

  Vector3 QuaternionMultiply(Quaternion q, Vector3 v)
  {
    Vector3 qVec = new Vector3(q.X, q.Y, q.Z);
    Vector3 t = 2.0f * Vector3.Cross(qVec, v);
    return v + q.W * t + Vector3.Cross(qVec, t);
  }
}

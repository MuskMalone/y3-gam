//using System.Numerics;
//using System;
//using IGE.Utils;

//public class Ladder : Entity
//{
//    public PlayerMove playerMove;
//    public PlayerInteraction playerInteraction;
//    private bool isOnLadder = false;
//    private float ladderSpeed = 30f; // Adjust speed as needed
//    private float maxLadderHeight = 55f; // Max Y height player can climb
//    public Entity invisibleBarrier;
//    public Entity ClickUI;

//    public Ladder() : base()
//    {
//    }

//    void Start()
//    {
//        //invisibleBarrier.SetActive(true);
//        if (playerMove == null)
//        {
//            Debug.LogError("[Ladder.cs] PlayerMove Script Entity not found!");
//            return;
//        }
//        ClickUI?.SetActive(false);
//    }

//    void Update()
//    {
//        bool mouseClicked = Input.GetMouseButtonTriggered(0);
//        bool isLadderHit = playerInteraction.RayHitString == "Ladder";
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (mouseClicked && (isLadderHit || playerPosition.Y >= maxLadderHeight))
//        {
//            ToggleLadderState();
//        }

//        if (isOnLadder)
//        {
//            HandleLadderMovement();
//        }
//    }

//    private void ToggleLadderState()
//    {
//        isOnLadder = !isOnLadder;
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (isOnLadder)
//        {
//            playerMove.FreezePlayer(); // Disable normal movement
//            playerMove.canLook = true; // Allow looking around
//            InternalCalls.SetGravityFactor(playerMove.mEntityID, 0f); // Disable gravity while climbing
//        }
//        else
//        {
//            // Check if the player is at the max height before getting off
//            if (playerPosition.Y >= maxLadderHeight)
//            {
//                // Move the player slightly forward to ensure they land on the platform
//                Vector3 forwardOffset = playerMove.GetComponent<Transform>().forward * 8.0f; // Adjust offset if needed
//                playerPosition += forwardOffset;

//                // Set new position so they land on the platform instead of falling
//                InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
//            }

//            // Restore normal movement
//            playerMove.UnfreezePlayer();
//            InternalCalls.SetGravityFactor(playerMove.mEntityID, playerMove.initialGravityFactor);
//        }
//    }


//    private void HandleLadderMovement()
//    {
//        float verticalInput = Input.GetAxis("Vertical"); // W (1), S (-1)
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (Math.Abs(verticalInput) > float.Epsilon) // Only move if input is given
//        {
//            // Prevent climbing above max height
//            if (verticalInput > 0 && playerPosition.Y >= maxLadderHeight)
//            {
//                return; // Stop upward movement at max height
//            }

//            // Modify only the Y position (up/down)
//            playerPosition.Y += verticalInput * ladderSpeed * InternalCalls.GetDeltaTime();

//            // Ensure player does not go above max height
//            if (playerPosition.Y > maxLadderHeight)
//            {
//                playerPosition.Y = maxLadderHeight;
//            }

//            InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

//            // Optionally, apply a slight downward force to prevent weird floaty behavior
//            InternalCalls.MoveCharacter(playerMove.mEntityID, new Vector3(0, -0.1f, 0));
//        }
//    }
//}
using System;
using System.Numerics;
using IGE.Utils;

public class Ladder : Entity
{
  public PlayerMove playerMove;
  public PlayerInteraction playerInteraction;
  private bool isOnLadder = false;
  private float ladderSpeed = 30f; // Adjust speed as needed
  private float maxLadderHeight = 55f; // Max Y height player can climb
  public Entity invisibleBarrier;
  public Entity ClickUI;  // This UI will show when hovering over the ladder or at the top

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
      if (isLadderHit)
        ClickUI?.SetActive(true);
      else
        ClickUI?.SetActive(false);
    }
    else
    {
      // When on the ladder, if the player is at (or above) max height, show the ClickUI.
      if (playerPosition.Y >= maxLadderHeight)
        ClickUI?.SetActive(true);
      else
        ClickUI?.SetActive(false);
    }

    // Toggle ladder state if the player clicks while hovering over the ladder or when at the top.
    if (mouseClicked && (isLadderHit || playerPosition.Y >= maxLadderHeight))
    {
      ToggleLadderState();
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
    }
    else
    {
      // When leaving the ladder, if at the top, offset the player forward so they land on the platform.
      if (playerPosition.Y >= maxLadderHeight)
      {
        Vector3 forwardOffset = playerMove.GetComponent<Transform>().forward * 8.0f; // Adjust offset if needed
        playerPosition += forwardOffset;
        InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
      }
      playerMove.UnfreezePlayer();
      InternalCalls.SetGravityFactor(playerMove.mEntityID, playerMove.initialGravityFactor);
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
      // Prevent climbing above max height.
      if (verticalInput > 0 && playerPosition.Y >= maxLadderHeight)
        return;

      // Only modify the Y position.
      playerPosition.Y += verticalInput * ladderSpeed * InternalCalls.GetDeltaTime();

      if (playerPosition.Y > maxLadderHeight)
        playerPosition.Y = maxLadderHeight;

      InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

      // Optionally apply a slight downward force to stabilize movement.
      InternalCalls.MoveCharacter(playerMove.mEntityID, new Vector3(0, -0.1f, 0));
    }
  }
}

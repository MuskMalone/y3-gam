//    public Ladder() : base()
//    {
//    }

//    void Start()
//    {
//        if (playerMove == null)
//        {
//            Debug.LogError("[Ladder.cs] PlayerMove Script Entity not found!");
//            return;
//        }
//        // Initially hide the ClickUI.
//        ClickUI?.SetActive(false);
//    }

//    void Update()
//    {
//        bool mouseClicked = Input.GetMouseButtonTriggered(0);
//        bool isLadderHit = playerInteraction.RayHitString == "Ladder";
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        // When not on the ladder, show the ClickUI if the player's ray is hitting the ladder.
//        if (!isOnLadder)
//        {
//            ClickUI?.SetActive(isLadderHit);
//        }
//        else
//        {
//            // When on the ladder, show the ClickUI if at the top or bottom.
//            if (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight)
//                ClickUI?.SetActive(true);
//            else
//                ClickUI?.SetActive(false);
//        }

//        // Toggle ladder state:
//        // - If NOT on the ladder, require the player to be looking at the ladder (isLadderHit).
//        // - If already on the ladder, allow toggling if at the top or bottom (max or min height).
//        if (mouseClicked)
//        {
//            if (!isOnLadder && isLadderHit)
//            {
//                ToggleLadderState();
//            }
//            else if (isOnLadder && (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight))
//            {
//                ToggleLadderState();
//            }
//        }

//        if (isOnLadder)
//        {
//            HandleLadderMovement();
//        }
//    }

//    private void ToggleLadderState()
//    {
//        // Toggle the ladder state.
//        isOnLadder = !isOnLadder;
//        playerMove.climbing = isOnLadder;
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (isOnLadder)
//        {
//            // When starting to climb, hide the ClickUI and disable normal movement.
//            ClickUI?.SetActive(false);
//            playerMove.FreezePlayer();
//            playerMove.canLook = true;
//            InternalCalls.SetGravityFactor(playerMove.mEntityID, 0f);
//        }
//        else
//        {
//            // When leaving the ladder, if at the top, offset the player forward so they land on the platform.
//            if (playerPosition.Y >= maxLadderHeight)
//            {
//                Vector3 forwardOffset = playerMove.GetComponent<Transform>().forward * 8.0f; // Adjust offset if needed
//                playerPosition += forwardOffset;
//                InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
//            }
//            playerMove.UnfreezePlayer();
//            InternalCalls.SetGravityFactor(playerMove.mEntityID, playerMove.initialGravityFactor);
//            // Hide the ClickUI when not on the ladder.
//            ClickUI?.SetActive(false);
//        }
//    }

//    private void HandleLadderMovement()
//    {
//        float verticalInput = Input.GetAxis("Vertical"); // W (1) for up, S (-1) for down
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (Math.Abs(verticalInput) > float.Epsilon)
//        {
//            // Prevent climbing above max height.
//            if (verticalInput > 0 && playerPosition.Y >= maxLadderHeight)
//                return;

//            // Only modify the Y position.
//            playerPosition.Y += verticalInput * ladderSpeed * InternalCalls.GetDeltaTime();

//            if (playerPosition.Y > maxLadderHeight)
//                playerPosition.Y = maxLadderHeight;

//            InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

//            // Optionally apply a slight downward force to stabilize movement.
//            InternalCalls.MoveCharacter(playerMove.mEntityID, new Vector3(0, -0.1f, 0));
//        }
//    }
//}



//----------------------------------
//using System;
//using System.Numerics;
//using IGE.Utils;

//public class Ladder : Entity
//{
//    public PlayerMove playerMove;
//    public PlayerInteraction playerInteraction;
//    private bool isOnLadder = false;
//    public float ladderSpeed = 30f; // Adjust speed as needed
//    public float maxLadderHeight = 55f; // Max Y height player can climb
//    public float minLadderHeight = 0f;  // Min Y height (bottom of the ladder)
//    public Entity ClickUI;  // This UI will show when hovering over the ladder or at the top/bottom

//    public Ladder() : base()
//    {
//    }

//    void Start()
//    {
//        if (playerMove == null)
//        {
//            Debug.LogError("[Ladder.cs] PlayerMove Script Entity not found!");
//            return;
//        }
//        // Initially hide the ClickUI.
//        ClickUI?.SetActive(false);
//    }

//    void Update()
//    {
//        bool mouseClicked = Input.GetMouseButtonTriggered(0);
//        bool isLadderHit = playerInteraction.RayHitString == "Ladder";
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        // When not on the ladder, show the ClickUI if the player's ray is hitting the ladder.
//        if (!isOnLadder)
//        {
//            ClickUI?.SetActive(isLadderHit);
//        }
//        else
//        {
//            // When on the ladder, show the ClickUI if at the top or bottom.
//            if (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight)
//                ClickUI?.SetActive(true);
//            else
//                ClickUI?.SetActive(false);
//        }

//        // Toggle ladder state:
//        // - If NOT on the ladder, require the player to be looking at the ladder (isLadderHit).
//        // - If already on the ladder, allow toggling if at the top or bottom (max or min height).
//        if (mouseClicked)
//        {
//            if (!isOnLadder && isLadderHit)
//            {
//                ToggleLadderState();
//            }
//            else if (isOnLadder && (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight))
//            {
//                ToggleLadderState();
//            }
//        }

//        if (isOnLadder)
//        {
//            HandleLadderMovement();
//        }
//    }

//    private void ToggleLadderState()
//    {
//        // Toggle the ladder state.
//        isOnLadder = !isOnLadder;
//        playerMove.climbing = isOnLadder;
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (isOnLadder)
//        {
//            // When starting to climb, hide the ClickUI and disable normal movement.
//            ClickUI?.SetActive(false);
//            playerMove.FreezePlayer();
//            playerMove.canLook = true;
//            InternalCalls.SetGravityFactor(playerMove.mEntityID, 0f);
//        }
//        else
//        {
//            // When leaving the ladder, if at the top, offset the player so they land on the platform.
//            if (playerPosition.Y >= maxLadderHeight)
//            {
//                // Check what the player is looking at.
//                // If the player's raycast is hitting a wall or ladder, launch them backwards.
//                Vector3 direction;
//                if (playerInteraction.RayHitString == "Wall" || playerInteraction.RayHitString == "Ladder")
//                {
//                    direction = -playerMove.GetComponent<Transform>().forward;
//                }
//                else
//                {
//                    direction = playerMove.GetComponent<Transform>().forward;
//                }
//                Vector3 offset = direction * 8.0f; // Adjust offset if needed
//                playerPosition += offset;
//                InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
//            }
//            playerMove.UnfreezePlayer();
//            InternalCalls.SetGravityFactor(playerMove.mEntityID, playerMove.initialGravityFactor);
//            // Hide the ClickUI when not on the ladder.
//            ClickUI?.SetActive(false);
//        }
//    }

//    private void HandleLadderMovement()
//    {
//        float verticalInput = Input.GetAxis("Vertical"); // W (1) for up, S (-1) for down
//        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

//        if (Math.Abs(verticalInput) > float.Epsilon)
//        {
//            // Prevent climbing above max height.
//            if (verticalInput > 0 && playerPosition.Y >= maxLadderHeight)
//                return;

//            // Only modify the Y position.
//            playerPosition.Y += verticalInput * ladderSpeed * InternalCalls.GetDeltaTime();

//            if (playerPosition.Y > maxLadderHeight)
//                playerPosition.Y = maxLadderHeight;

//            InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

//            // Optionally apply a slight downward force to stabilize movement.
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
    public float ladderSpeed = 30f; // Adjust speed as needed
    public float maxLadderHeight = 55f; // Max Y height player can climb
    public float minLadderHeight = 0f;  // Min Y height (bottom of the ladder)
    public Entity ClickUI;  // This UI will show when hovering over the ladder or at the top/bottom

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
                ToggleLadderState();
            }
            else if (isOnLadder && (playerPosition.Y >= maxLadderHeight || playerPosition.Y <= minLadderHeight))
            {
                ToggleLadderState();
            }
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
            // When leaving the ladder, if at the top, offset the player so they land on the platform.
            if (playerPosition.Y >= maxLadderHeight)
            {
                Vector3 direction;
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
            // **New Code: If at the bottom, ensure the player is placed above the floor.**
            if (playerPosition.Y <= minLadderHeight)
            {
                // Adjust the player's Y so that they don't get stuck below the floor.
                playerPosition.Y = minLadderHeight + 1.0f; // Adjust offset as needed.
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

            // Prevent climbing below the minimum ladder height.
            if (verticalInput < 0 && playerPosition.Y <= minLadderHeight)
                return;

            // Only modify the Y position.
            playerPosition.Y += verticalInput * ladderSpeed * InternalCalls.GetDeltaTime();

            // Clamp Y position to valid bounds.
            if (playerPosition.Y > maxLadderHeight)
                playerPosition.Y = maxLadderHeight;
            if (playerPosition.Y < minLadderHeight)
                playerPosition.Y = minLadderHeight;

            InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

            // Optionally apply a slight downward force to stabilize movement.
            InternalCalls.MoveCharacter(playerMove.mEntityID, new Vector3(0, -0.1f, 0));
        }
    }
}
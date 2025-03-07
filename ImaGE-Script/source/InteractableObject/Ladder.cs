using System.Numerics;
using System;
using IGE.Utils;

public class Ladder : Entity
{
    public PlayerMove playerMove;
    public PlayerInteraction playerInteraction;
    private bool isOnLadder = false;
    private float ladderSpeed = 30f; // Adjust speed as needed
    private float maxLadderHeight = 55f; // Max Y height player can climb
    public Entity invisibleBarrier;

    public Ladder() : base()
    {
    }

    void Start()
    {
        invisibleBarrier.SetActive(false);
        if (playerMove == null)
        {
            Debug.LogError("[Ladder.cs] PlayerMove Script Entity not found!");
            return;
        }
    }

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        bool isLadderHit = playerInteraction.RayHitString == "Ladder";
        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

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
        isOnLadder = !isOnLadder;
        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

        if (isOnLadder)
        {
            playerMove.FreezePlayer(); // Disable normal movement
            playerMove.canLook = true; // Allow looking around
            InternalCalls.SetGravityFactor(playerMove.mEntityID, 0f); // Disable gravity while climbing
        }
        else
        {
            // Check if the player is at the max height before getting off
            if (playerPosition.Y >= maxLadderHeight)
            {
                // Move the player slightly forward to ensure they land on the platform
                Vector3 forwardOffset = playerMove.GetComponent<Transform>().forward * 8.0f; // Adjust offset if needed
                playerPosition += forwardOffset;

                // Set new position so they land on the platform instead of falling
                InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);
            }

            // Restore normal movement
            playerMove.UnfreezePlayer();
            InternalCalls.SetGravityFactor(playerMove.mEntityID, playerMove.initialGravityFactor);
        }
    }


    private void HandleLadderMovement()
    {
        float verticalInput = Input.GetAxis("Vertical"); // W (1), S (-1)
        Vector3 playerPosition = InternalCalls.GetWorldPosition(playerMove.mEntityID);

        if (Math.Abs(verticalInput) > float.Epsilon) // Only move if input is given
        {
            // Prevent climbing above max height
            if (verticalInput > 0 && playerPosition.Y >= maxLadderHeight)
            {
                return; // Stop upward movement at max height
            }

            // Modify only the Y position (up/down)
            playerPosition.Y += verticalInput * ladderSpeed * InternalCalls.GetDeltaTime();

            // Ensure player does not go above max height
            if (playerPosition.Y > maxLadderHeight)
            {
                playerPosition.Y = maxLadderHeight;
            }

            InternalCalls.SetWorldPosition(playerMove.mEntityID, ref playerPosition);

            // Optionally, apply a slight downward force to prevent weird floaty behavior
            InternalCalls.MoveCharacter(playerMove.mEntityID, new Vector3(0, -0.1f, 0));
        }
    }
}

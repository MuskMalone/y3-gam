////using System.Collections;
////using System.Collections.Generic;
////using UnityEngine;

////public class PlayerMotor : Entity
////{
////    AudioManager audioManager;
////    private CharacterController controller;
////    private Vector3 playerVelocity;
////    private bool isGrounded;
////    public float speed = 5.0f;
////    public float gravity = -9.8f;
////    public float jumpHeight = 3.0f;

////    // New flag to control whether the player can move or not
////    public bool canMove = true;

////    private void Awake()
////    {
////        audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
////    }

////    void Start()
////    {
////        controller = GetComponent<CharacterController>();
////    }

////    void Update()
////    {
////        isGrounded = controller.isGrounded;
////        if (canMove && isGrounded && (Input.GetAxis("Horizontal") != 0 || Input.GetAxis("Vertical") != 0))
////        {
////            PlayFootstepSound();
////        }
////        else
////        {

////        }

////    }

////    void PlayFootstepSound()
////    {

////        RaycastHit hit;
////        Ray ray = new Ray(transform.position, -Vector3.up);

////        if (Physics.Raycast(ray, out hit, 2f))
////        {
////            //Debug.Log("test");
////            string layerName = LayerMask.LayerToName(hit.collider.Entity.layer);

////            switch (layerName)
////            {
////                case "Metal Pipes":
////                    Debug.Log("playing metal pipes");
////                    audioManager.PlayFootsteps(audioManager.metalpipe);
////                    break;
////                case "Pavement":
////                    Debug.Log("playing pavement");
////                    audioManager.PlayFootsteps(audioManager.pavement);
////                    break;
////                default:
////                    Debug.Log("playing grass");
////                    audioManager.PlayFootsteps(audioManager.grass);
////                    break;
////            }
////        }
////    }

////    // Method to process player movement, now checking if the player can move
////    public void ProcessMove(Vector2 input)
////    {
////        if (!canMove) return;  // Skip movement if the player is frozen

////        Vector3 moveDirection = Vector3.zero;
////        moveDirection.x = input.x;
////        moveDirection.z = input.y;
////        controller.Move(transform.TransformDirection(moveDirection) * speed * Time.deltaTime);

////        playerVelocity.y += gravity * Time.deltaTime;
////        if (isGrounded && playerVelocity.y < 0)
////        {
////            playerVelocity.y = -2.0f;
////        }
////        controller.Move(playerVelocity * Time.deltaTime);
////    }

////    public void Jump()
////    {
////        if (isGrounded && canMove)  // Check canMove to prevent jumping when frozen
////        {
////            playerVelocity.y = Mathf.Sqrt(jumpHeight * -1.0f * gravity);
////        }
////    }
////}


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Image.Utils;

public class PlayerMotor : Entity
{
    AudioManager audioManager;
    private CharacterController controller;
    private Vector3 playerVelocity;
    private bool isGrounded;
    public float speed = 5.0f;
    public float gravity = -9.8f;
    public float jumpHeight = 3.0f;

    // New flag to control whether the player can move or not
    public bool canMove = true;

    // Footstep timing variables
    private float stepCooldown = 1f;
    private float stepTimer = 0f; 

    private void Awake()
    {
        audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
    }

    void Start()
    {
        controller = GetComponent<CharacterController>();
    }

    void Update()
    {
        isGrounded = controller.isGrounded;

        // Handle movement and footstep sounds based on player input
        if (canMove && isGrounded && (Input.GetAxis("Horizontal") != 0 || Input.GetAxis("Vertical") != 0))
        {
            stepTimer -= Time.deltaTime;

            if (stepTimer <= 0f)
            {
                PlayFootstepSound();
                stepTimer = stepCooldown;
            }
        }
        else
        {
            // Stop footstep sound immediately when player stops moving
            audioManager.StopFootsteps();
            stepTimer = 0f;  // Reset step timer
        }
    }

    void PlayFootstepSound()
    {
        RaycastHit hit;
        Ray ray = new Ray(GetComponent<Transform>().position, new Vector3(0.0f, -1.0f, 0.0f));

        if (Physics.Raycast(ray, out hit, 2f))
        {
            string layerName = LayerMask.LayerToName(hit.collider.Entity.layer);

            // Choose the correct footstep sound based on the surface the player is walking on
            switch (layerName)
            {
                case "Metal Pipes":
                    audioManager.PlayRandomMetalPipesFootstep();
                    break;
                case "Pavement":
                    audioManager.PlayRandomPavementFootstep();
                    break;
                case "Soul Pipes":
                    audioManager.PlayRandomMetalPipesFootstep();
                    audioManager.PlayRandomScreams();
                    break;
                default:
                    // Play random grass footstep when walking on grass
                    audioManager.PlayRandomGrassFootstep();
                    break;
            }
        }
    }

    // Method to process player movement, now checking if the player can move
    public void ProcessMove(Vector2 input)
    {
        if (!canMove) return;  // Skip movement if the player is frozen

        Vector3 moveDirection = new Vector3();
        moveDirection.X = input.X;
        moveDirection.Z = input.Y;
        controller.Move(GetComponent<Transform>().TransformDirection(moveDirection) * speed * Time.deltaTime);

        playerVelocity.Y += gravity * Time.deltaTime;
        if (isGrounded && playerVelocity.Y < 0)
        {
            playerVelocity.Y = -2.0f;
        }
        controller.Move(playerVelocity * Time.deltaTime);
    }

    public void Jump()
    {
        if (isGrounded && canMove)
        {
            playerVelocity.Y = Mathf.Sqrt(jumpHeight * -2.0f * gravity);
        }
    }
}

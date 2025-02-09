using System;
using IGE.Utils;
using System.Numerics;

public class PlayerFootsteps : Entity
{
    //public Entity audioManager;
    public Entity player;
    public PlayerMove playerMoveScript;
    public float interval = 0.5f;
    // Start is called before the first frame update
    private float timePassed;
    private Random random = new Random();
    public PlayerFootsteps() : base()
    {

    }
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        timePassed += InternalCalls.GetDeltaTime();
        bool isGrounded = playerMoveScript.IsGrounded();
        Vector3 velocity = InternalCalls.GetVelocity(player.mEntityID);
        float magnitude = velocity.X * velocity.X + velocity.Y * velocity.Y + velocity.Z * velocity.Z;

        //for testing purposes to remove
        Vector3 position = InternalCalls.GetPosition(player.mEntityID);
        uint entityHit = InternalCalls.RaycastFromEntity(player.mEntityID, position, position + (new Vector3(0, -200, 0)));

        if (isGrounded && magnitude > 0.1f)
        {
            if (timePassed >= interval) {
                PlayFootstepSound();
                timePassed = 0;
            }
        }
    }

    public void PlayRandomMetalSound()
    {
        int soundNumber = random.Next(1, 4);
        string soundName = $"MetalPipe{soundNumber}";
        InternalCalls.PlaySound(mEntityID, soundName);
    }
    public void PlayRandomGrassSound()
    {
        int soundNumber = random.Next(1, 7);
        string soundName = $"Grass{soundNumber}";
        InternalCalls.PlaySound(mEntityID, soundName);
    }

    public void PlayRandomPavementSound()
    {
        int soundNumber = random.Next(1, 6);
        string soundName = $"Pavement{soundNumber}";
        InternalCalls.PlaySound(mEntityID, soundName);
    }

    public void PlayRandomWoodenSound()
    {
        int soundNumber = random.Next(1, 6);
        string soundName = $"Wooden{soundNumber}";
        InternalCalls.PlaySound(mEntityID, soundName);
    }
    void PlayFootstepSound()
    {
        // Raycast downward to detect the ground surface
        Vector3 position = InternalCalls.GetPosition(player.mEntityID);
        Vector3 scale = InternalCalls.GetScale(player.mEntityID);
        uint entityHit = InternalCalls.RaycastFromEntity(player.mEntityID, position, position + (new Vector3(0, -200, 0)));

        if (entityHit != 0)
        {
            // Check the layer name of the object hit by the raycast
            string tag = InternalCalls.GetTag(entityHit);

            switch (tag)
            {
                case "MainGround":
                    PlayRandomGrassSound();
                    Debug.Log("GrassSound");
                    break;
                case "Second Level":
                    PlayRandomPavementSound();
                    break;
                case "Pit Room Ground":
                    PlayRandomPavementSound();
                    break;
                case "Pit Room Platform":
                    PlayRandomPavementSound();
                    break;
                case "Stair Head":
                    PlayRandomPavementSound();
                    break;
                case "Metal Pipes":
                    PlayRandomMetalSound();
                    break;
                case "Wooden Ground":
                    PlayRandomWoodenSound();
                    break;
                default:
                    Debug.Log("No Sound Case " + tag);
                    break;
            }
        }
    }
}

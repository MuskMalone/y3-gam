using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;
using System.Numerics;

public class PlayerFootsteps : Entity
{
    //public Entity audioManager;
    public Entity player;
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
        bool isGrounded = InternalCalls.IsGrounded(player.mEntityID);
        Vector3 velocity = InternalCalls.GetVelocity(player.mEntityID);
        float magnitude = velocity.X * velocity.X + velocity.Y * velocity.Y + velocity.Z * velocity.Z;
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
        Debug.Log("grass sound");
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
    void PlayFootstepSound()
    {
        // Raycast downward to detect the ground surface
        Vector3 position = InternalCalls.GetPosition(player.mEntityID);
        Vector3 scale = InternalCalls.GetScale(player.mEntityID);
        uint entityHit = InternalCalls.Raycast(position, position + (new Vector3(0, -2, 0)));
        Debug.Log($"position {position.X} {position.Y} {position.Z}");
        if (entityHit != 0)
        {
            // Check the layer name of the object hit by the raycast
            string tag = InternalCalls.GetTag(entityHit);

            switch (tag)
            {
                case "MainGround":
                    PlayRandomGrassSound();
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
                default:
                    Debug.Log("No Sound Case " + tag);
                    break;
            }
        }
    }
}

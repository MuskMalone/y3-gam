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
    // Start is called before the first frame update
    public PlayerFootsteps() : base()
    {

    }
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        bool isGrounded = InternalCalls.IsGrounded(player.mEntityID);
        Vector3 velocity = InternalCalls.GetVelocity(player.mEntityID);
        float magnitude = velocity.X * velocity.X + velocity.Y * velocity.Y + velocity.Z * velocity.Z;
        Debug.Log("amagnitude " + magnitude.ToString());
        if (isGrounded && magnitude > 0.1f)
        {
            PlayFootstepSound();
        }
    }

    void PlayFootstepSound()
    {
        // Raycast downward to detect the ground surface
        Vector3 position = InternalCalls.GetPosition(player.mEntityID);
        uint entityHit = InternalCalls.Raycast(position, position + (new Vector3(0, -1, 0)));
        if (entityHit != 0)
        {
            // Check the layer name of the object hit by the raycast
            string tag = InternalCalls.GetTag(entityHit);

            switch (tag)
            {
                case "Stair Head":
                    Debug.Log("Stepping on metal Pipes");
                    InternalCalls.PlaySound(mEntityID, "Pavement");
                    break;
                
                default:
                    Debug.Log("No Sound");
                    break;
            }
        }
    }
}

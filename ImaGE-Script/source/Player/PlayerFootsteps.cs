//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;

//public class PlaerFootsteps : Entity
//{
//    AudioManager audioManager;
//    public Entity 

//    private void Awake()
//    {
//        audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
//    }

//    // Start is called before the first frame update
//    void Start()
//    {

//    }

//    // Update is called once per frame
//    void Update()
//    {
//        if (player.isGrounded && player.velocity.magnitude > 0.1f)
//        {

//        }
//    }

//    void PlayFootstepSound()
//    {
//        // Raycast downward to detect the ground surface
//        Ray ray = new Ray(transform.position, Vector3.down);
//        RaycastHit hit;

//        if (Physics.Raycast(ray, out hit, 2f))
//        {
//            // Check the layer name of the object hit by the raycast
//            string layerName = LayerMask.LayerToName(hit.collider.Entity.layer);

//            switch (layerName)
//            {
//                case "Metal Pipes":
//                    Debug.Log("Stepping on metal Pipes");
//                    audioManager.PlaySFX(audioManager.metalpipe);
//                    break;

//                default:
//                    // Optionally handle default case if needed
//                    break;
//            }
//        }
//    }
//}

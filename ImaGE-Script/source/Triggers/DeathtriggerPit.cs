//using System;
//using System.Collections;
//using System.Collections.Generic;
//using System.Linq;
//using System.Reflection;
//using System.Runtime.CompilerServices;
//using System.Text;
//using System.Threading.Tasks;
//using System.Xml;
//using Image.Utils;


//  public class DeathtriggerPit : Entity
//{
//    public Transform player;
//    public Transform startPosition; // The starting point to reset the player to
//    public Transform finishPosition;
//    private FinishLineTrigger finishlineScript;

//    void Start()
//    {
//        finishlineScript = FindObjectOfType<FinishLineTrigger>();
//    }

//    private void OnTriggerStay(Collider other)
//    {
//        if (other.CompareTag("Player"))
//        {
//            Debug.Log("Player has fallen into the pit!");
//            CharacterController controller = player.GetComponent<CharacterController>();

//            if (controller != null)
//            {
//                // Temporarily disable the controller to move the player
//                controller.enabled = false;

//                if (finishlineScript.crossFinish)
//                {
//                    player.position = finishPosition.position;
//                }
//                else
//                {
//                    // Reset the player's position to the start position
//                    player.position = startPosition.position;
//                }
                

//                // Re-enable the CharacterController
//                controller.enabled = true;
//            }
//        }
//    }
//}
  

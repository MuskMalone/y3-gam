//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;

//public class PlayerInteraction : Entity
//{
//    private PictureAlign alignScript;
//    private PlayerMotor playerMotor;     
//    private PlayerLook playerLook;       

//    public bool isFrozen = false;      

//    // Start is called before the first frame update
//    void Start()
//    {
//        playerMotor = FindObjectOfType<PlayerMotor>();
//        playerLook = FindObjectOfType<PlayerLook>();
//        alignScript = FindObjectOfType<PictureAlign>();
//    }

//    // Update is called once per frame
//    void Update()
//    {
//        if (alignScript != null)
//        {

//            if (alignScript.alignCheck && Input.GetMouseButtonDown(0) && !isFrozen)
//            {
//                FreezePlayer();
//                isFrozen = true;
//            }

//        }
//        else
//        {
//            Debug.LogError("alignScript reference is missing.");
//        }
//    }

//    void FreezePlayer()
//    {
//        if (playerMotor != null)
//        {
//            playerMotor.canMove = false;  
//        }
//        if (playerLook != null)
//        {
//            playerLook.canLook = false; 
//        }
//        Debug.Log("Player movement and look frozen.");
//    }

//    public void UnfreezePlayer()
//    {
//        if (playerMotor != null)
//        {
//            playerMotor.canMove = true; 
//        }
//        if (playerLook != null)
//        {
//            playerLook.canLook = true;  
//        }
//        Debug.Log("Player movement and look unfrozen.");
//    }
//}

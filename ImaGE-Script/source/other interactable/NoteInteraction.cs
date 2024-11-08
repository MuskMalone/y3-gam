////using System.Collections;
////using System.Collections.Generic;
////using UnityEngine;
////using static UnityEditor.Progress;

////public class NoteInteraction : Entity
////{
////    public bool playerInRange;

////    public string ItemName;

////    public Entity NoteUI;
////    //private Inventory inventoryScript;

////    //private HoldupUI holdupUI;
////    private SeedPlanter seedPlanter;
////    //private IInventoryItem inventoryItem;

////    private bool IsClicked;
////    /*public Sprite itemIcon;*/ // Add a reference to the item's icon
////    public string GetItemName()
////    {
////        return ItemName;
////    }

////    private void Start()
////    {
////        seedPlanter = FindObjectOfType<SeedPlanter>();
////        //holdupUI = FindObjectOfType<HoldupUI>();
////        //inventoryScript = FindObjectOfType<Inventory>();
////        IsClicked = false;
////    }

////    void Update()
////    {
////        if (Input.GetMouseButtonDown(0) && playerInRange && SelectionManager.Instance.onTarget && IsClicked == false)
////        {
////            NoteUI.SetActive(true);

////            //IInventoryItem item = Entity.GetComponent<IInventoryItem>();
////            //if (item != null)
////            //{
////            //    //if (item.Name == "Pit Painting") // You can modify this check as needed
////            //    //{
////            //    //    holdupUI.SetAssociatedItem(item); // Only call this if the item is a Pit Painting
////            //    //}
////            //    //else if (item.Name == "Seed") // You can modify this check as needed
////            //    //{
////            //    //    seedPlanter.SetAssociatedItem(item); // Only call this if the item is a Pit Painting
////            //    //}

////            //    //inventoryScript.Additem(item);


////            //}


////        }

////        if(IsClicked && Input.GetMouseButtonDown(0))
////        {

////            NoteUI.SetActive(true);
////        }

////    }
////    private void OnTriggerEnter(Collider other)
////    {
////        if (other.CompareTag("Player"))
////        {
////            playerInRange = true;
////        }
////    }

////    private void OnTriggerExit(Collider other)
////    {
////        if (other.CompareTag("Player"))
////        {
////            playerInRange = false;
////        }
////    }
////}

//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;

//public class NoteInteraction : Entity
//{
//    public bool playerInRange;
//    public string ItemName;
//    public Entity NoteUI;

//    private bool isNoteVisible = false;      // Track if the note is currently visible
//    private bool isFrozen = false;

//    private PlayerMotor playerMotor;
//    private PlayerLook playerLook;

//    public string GetItemName()
//    {
//        return ItemName;
//    }

//    private void Start()
//    {
//        // Ensure that Note UI is disabled at the start
//        if (NoteUI != null)
//        {
//            NoteUI.SetActive(false);
//        }


//        playerMotor = FindObjectOfType<PlayerMotor>();
//        playerLook = FindObjectOfType<PlayerLook>();

//        if (playerMotor == null) Debug.LogError("PlayerMotor component not found!");
//        if (playerLook == null) Debug.LogError("PlayerLook component not found!");
//    }

//    private void Update()
//    {
//        // Check if the player is interacting with the note and display the UI
//        if (Input.GetMouseButtonDown(0) && playerInRange && SelectionManager.Instance.onTarget && !isNoteVisible)
//        {
//            ShowNoteUI();
//        }
//        // If the player is frozen, check for a click to unfreeze and hide the UI
//        else if (isFrozen && Input.GetMouseButtonDown(0))
//        {
//            HideNoteUI();
//        }
//    }

//    // Show the Note UI and freeze the player
//    private void ShowNoteUI()
//    {
//        if (NoteUI != null)
//        {
//            NoteUI.SetActive(true);
//            isNoteVisible = true;
//            FreezePlayer();
//            isFrozen = true;
//        }
//    }

//    // Hide the Note UI and unfreeze the player
//    private void HideNoteUI()
//    {
//        if (NoteUI != null)
//        {
//            NoteUI.SetActive(false);
//            isNoteVisible = false;
//            UnfreezePlayer();
//            isFrozen = false;
//        }
//    }

//    private void OnTriggerEnter(Collider other)
//    {
//        if (other.CompareTag("Player"))
//        {
//            playerInRange = true;
//        }
//    }

//    private void OnTriggerExit(Collider other)
//    {
//        if (other.CompareTag("Player"))
//        {
//            playerInRange = false;
//            HideNoteUI();  // Hide the UI when the player leaves range and ensure unfreezing
//        }
//    }

//    // Freeze player movement and looking
//    public void FreezePlayer()
//    {
//        if (playerMotor != null)
//        {
//            playerMotor.canMove = false;  // Freeze player movement
//            Debug.Log("Player movement frozen.");
//        }

//        if (playerLook != null)
//        {
//            playerLook.canLook = false;  // Freeze camera movement
//            Debug.Log("Player camera look frozen.");
//        }
//    }

//    // Unfreeze player movement and looking
//    public void UnfreezePlayer()
//    {
//        if (playerMotor != null)
//        {
//            playerMotor.canMove = true;  // Unfreeze player movement
//        }

//        if (playerLook != null)
//        {
//            playerLook.canLook = true;  // Unfreeze camera movement
//        }

//        Debug.Log("Player movement and look unfrozen.");
//    }
//}

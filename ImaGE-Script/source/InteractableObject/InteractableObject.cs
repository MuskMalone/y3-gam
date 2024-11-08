//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;
////using static UnityEditor.Progress;

//public class InteractableObject : Entity
//{
//    AudioManager audioManager;

//    public bool playerInRange;

//    public string ItemName;

//    private Inventory inventoryScript;

//    private HoldupUI holdupUI;
//    private SeedPlanter seedPlanter;
//    //private IInventoryItem inventoryItem;

//    /*public Sprite itemIcon;*/ // Add a reference to the item's icon

//    private void Awake()
//    {
//        audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
//    }

//    public string GetItemName()
//    {
//        return ItemName;
//    }

//    private void Start()
//    {
//        seedPlanter = FindObjectOfType<SeedPlanter>();
//        holdupUI = FindObjectOfType<HoldupUI>();
//        inventoryScript = FindObjectOfType<Inventory>();
//    }

//    void Update()
//    {
//        if (Input.GetKeyDown(KeyCode.E) && playerInRange && SelectionManager.Instance.onTarget)
//        {

//            IInventoryItem item = Entity.GetComponent<IInventoryItem>();
//            if (item != null)
//            {
//                audioManager.PlaySFX(audioManager.itemPickup);
//                inventoryScript.Additem(item);

//            }


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
//        }
//    }
//}



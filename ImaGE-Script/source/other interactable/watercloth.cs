using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Image.Utils;
public class watercloth : Entity
{
    public bool playerInRange;
    public string ItemName;
   

    
    private Inventory inventory;
    public bool isClothWet;


    public string GetItemName()
    {
        return ItemName;
    }

    private void Start()
    {
        
        isClothWet = false;
        inventory = FindObjectOfType<Inventory>();
        if (inventory == null) Debug.LogError("Inventory component not found!");
    }

    private void Update()
    {
        //detect mouse click player in range
        if (Input.GetMouseButtonDown(0) && playerInRange && SelectionManager.Instance.onTarget && inventory.isClothActive)
        {
            Debug.Log("Switch out dry cloth for damp cloth");
            isClothWet = true;
        }
        // If the player is frozen, check for a click to unfreeze and hide the UI
        //else if (isFrozen && Input.GetMouseButtonDown(0))
        //{
        //    HideNoteUI();
        //}
    }

    

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            playerInRange = true;
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            playerInRange = false;
          
        }
    }

   
    
}

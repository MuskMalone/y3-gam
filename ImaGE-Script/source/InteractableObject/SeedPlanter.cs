////using UnityEngine;

////public class SeedPlanter : Entity
////{
////    public Entity seedPrefab; // Reference to the seed prefab
////    private SkyboxController skyboxController; // Reference to the SkyboxController instance
////    private Inventory inventoryScript;
////    //private IInventoryItem associatedItem;
////    private void Start()
////    {
////        // Find the SkyboxController in the scene
////        skyboxController = FindObjectOfType<SkyboxController>();
////        inventoryScript = FindObjectOfType<Inventory>();
////    }


////    private void Update()
////    {
////        // If seed UI is active and player left-clicks

////        if (Entity.activeSelf && Input.GetMouseButtonDown(0))
////        {
////            Debug.Log("Click");
////            HandleSeedPlanting();

////            IInventoryItem itemToUse = inventoryScript.GetItemByName("Seed");
////            Debug.Log("itemToUse" + itemToUse);
////            if (itemToUse != null)
////            {
////                Debug.Log("name" + itemToUse.Name);
////                inventoryScript.RemoveItem(itemToUse); // Remove after use
////            }

////        }
////    }

////    private void HandleSeedPlanting()
////    {
////        // Perform a raycast to detect if we're looking at a cylinder
////        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
////        RaycastHit hit;

////        if (Physics.Raycast(ray, out hit, Mathf.Infinity))
////        {
////            if (hit.collider.Entity.name == "pot")
////            {
////                Debug.Log("Seed");

////                // Instantiate the seed prefab on the cylinder
////                if (seedPrefab != null)
////                {
////                    Vector3 spawnPosition = hit.point; // Spawn at the hit point on the cylinder
////                    Instantiate(seedPrefab, spawnPosition, Quaternion.identity);
////                }

////                // Use the class name directly to access the static property
////                SkyboxController.SeedPlantedFlag = true;

////                // Notify the SkyboxController that the seed is planted
////                if (skyboxController != null)
////                {
////                    skyboxController.HandleSeedPlanted(); // Inform SkyboxController of the seed planting
////                }

////                Debug.Log("Seed has been planted in the pot");

////                // Plant the seed and notify the SkyboxController
////                //Entity.SetActive(false); // Deactivate seed UI
////                Destroy(Entity);

////            }
////        }
////    }
////}
//using UnityEngine;

//public class SeedPlanter : Entity
//{
//    public Entity pot; // Reference to the seed prefab
//    private SkyboxController skyboxController; // Reference to the SkyboxController instance
//    private Inventory inventoryScript;

//    //public Entity pot;
//    public Entity potWithSeeds;

//    //private IInventoryItem associatedItem;
//    private void Start()
//    {
//        potWithSeeds.SetActive(false);
//        // Find the SkyboxController in the scene
//        skyboxController = FindObjectOfType<SkyboxController>();
//        inventoryScript = FindObjectOfType<Inventory>();
//    }


//    private void Update()
//    {
//        // If seed UI is active and player left-clicks

//        if (Entity.activeSelf && Input.GetMouseButtonDown(0))
//        {
//            Debug.Log("Click");
//            HandleSeedPlanting();

//            IInventoryItem itemToUse = inventoryScript.GetItemByName("Seed");
//            Debug.Log("itemToUse" + itemToUse);
//            if (itemToUse != null)
//            {
//                Debug.Log("name" + itemToUse.Name);
//                inventoryScript.RemoveItem(itemToUse); // Remove after use
//            }

//        }
//    }

//    private void HandleSeedPlanting()
//    {
//        // Perform a raycast to detect if we're looking at a cylinder
//        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
//        RaycastHit hit;

//        if (Physics.Raycast(ray, out hit, Mathf.Infinity))
//        {
//            if (hit.collider.Entity.name == "pot")
//            {
//                Debug.Log("Seed");

//                // Instantiate the seed prefab on the cylinder
//                //if (seedPrefab != null)
//                //{
//                //    Vector3 spawnPosition = hit.point; // Spawn at the hit point on the cylinder
//                //    Instantiate(seedPrefab, spawnPosition, Quaternion.identity);
//                //}

//                if (pot != null && potWithSeeds != null)
//                {
//                    pot.SetActive(false);   // Deactivate the empty pot
//                    potWithSeeds.SetActive(true); // Activate the pot with seeds
//                }

//                // Use the class name directly to access the static property
//                SkyboxController.SeedPlantedFlag = true;

//                // Notify the SkyboxController that the seed is planted
//                if (skyboxController != null)
//                {
//                    skyboxController.HandleSeedPlanted(); // Inform SkyboxController of the seed planting
//                }

//                Debug.Log("Seed has been planted in the pot");

//                // Plant the seed and notify the SkyboxController
//                //Entity.SetActive(false); // Deactivate seed UI
//                Destroy(Entity);

//            }
//        }
//    }
//}

//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using System.Drawing;      // For Bitmap (or use other graphics libraries)
//using System.Drawing.Imaging;
//using IGE.Utils;

//public class CleanWindows : Entity
//{
//  public bool playerInRange;
//  public string ItemName;
//  public string AltItemName;

//  public Material dirtMaterial;  // The material used for the dirt
//  public float fadeSpeed = 1f;   // Speed at which the dirt fades away
//  private bool isCleaning = false;  // Flag to check if the player is cleaning the window
//  private float currentAlpha = 1f;  // Current alpha value of the dirt
//  public bool hasCleanedWindow;

//  private Material dirtMaterialInstance;  // Instance of the dirt material
//  private Inventory inventory;
//  private watercloth waterCloth;

//  //dirty window
//  public string GetItemName()
//  {
//    return ItemName;
//  }
//  //clean window
//  public string GetAltItemName()
//  {
//    return AltItemName;
//  }

//  private void Start()
//  {
//    hasCleanedWindow = false;
//    // Create an instance of the dirt material for this window
//    dirtMaterialInstance = new Material(dirtMaterial);

//    // Apply the unique material instance to the renderer
//    GetComponent<Renderer>().material = dirtMaterialInstance;

//    // Set initial alpha
//    dirtMaterialInstance.color = new Color(dirtMaterialInstance.color.r, dirtMaterialInstance.color.g, dirtMaterialInstance.color.b, currentAlpha);

//    inventory = FindObjectOfType<Inventory>();
//    if (inventory == null) Debug.LogError("Inventory component not found!");

//    waterCloth = FindObjectOfType<watercloth>();
//    if (waterCloth == null) Debug.LogError("waterCloth component not found!");
//  }

//  private void Update()
//  {
//    // Detect mouse click player in range
//    if (Input.GetMouseButtonDown(0) && playerInRange && SelectionManager.Instance.onTarget && inventory.isClothActive && waterCloth.isClothWet && !hasCleanedWindow)
//    {
//      isCleaning = true;
//    }

//    if (isCleaning)
//    {
//      currentAlpha -= fadeSpeed * Time.deltaTime;
//      currentAlpha = Mathf.Clamp01(currentAlpha);  // Clamp between 0 and 1

//      // Update the material's transparency
//      dirtMaterialInstance.color = new Color(dirtMaterialInstance.color.r, dirtMaterialInstance.color.g, dirtMaterialInstance.color.b, currentAlpha);

//      // Stop cleaning when fully transparent
//      if (currentAlpha <= 0f)
//      {
//        hasCleanedWindow = true;
//        isCleaning = false;
//      }
//    }
//  }

//  private void OnTriggerEnter(Collider other)
//  {
//    if (other.CompareTag("Player"))
//    {
//      playerInRange = true;
//    }
//  }

//  private void OnTriggerExit(Collider other)
//  {
//    if (other.CompareTag("Player"))
//    {
//      playerInRange = false;
//    }
//  }
//}

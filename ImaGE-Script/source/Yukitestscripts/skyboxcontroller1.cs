

//////using UnityEngine;
//////using System.Collections;

//////public class SkyboxController : MonoBehaviour
//////{
//////    public Material Skybox_DualPanoramic; // Reference to the skybox material
//////    public Light directionalLight; // Reference to the directional light

//////    public float dayLightRotationX = 50f; // Rotation for day (morning)
//////    public float nightLightRotationX = -20f; // Rotation for night

//////    [Range(0, 1)] public float dayBlendValue = 0f; // Blend value for day
//////    [Range(0, 1)] public float nightBlendValue = 1f; // Blend value for night

//////    public float transitionSpeed = 1f; // Speed of the transition

//////    private float initialBlendValue; // Track initial blend value to restore during Play mode

//////    private void Awake()
//////    {
//////        // Save the initial blend value when the script is loaded
//////        initialBlendValue = Skybox_DualPanoramic.GetFloat("_Blend");
//////    }

//////    private void Start()
//////    {
//////        // Reset the blend value and directional light rotation to the default day state
//////        SetDaylight();
//////    }

//////    private void Update()
//////    {
//////        // Press 'N' to switch to night skybox and rotation
//////        if (Input.GetKeyDown(KeyCode.N))
//////        {
//////            StartCoroutine(SmoothTransition(nightBlendValue, nightLightRotationX));
//////        }

//////        // Press 'M' to switch to day skybox and rotation
//////        if (Input.GetKeyDown(KeyCode.M))
//////        {
//////            StartCoroutine(SmoothTransition(dayBlendValue, dayLightRotationX));
//////        }
//////    }

//////    // Coroutine to smoothly transition between day and night states
//////    private IEnumerator SmoothTransition(float targetBlend, float targetLightRotationX)
//////    {
//////        float startBlend = Skybox_DualPanoramic.GetFloat("_Blend");
//////        float startRotationX = directionalLight.transform.rotation.eulerAngles.x;
//////        float elapsedTime = 0f;

//////        while (elapsedTime < 1f)
//////        {
//////            elapsedTime += Time.deltaTime * transitionSpeed;

//////            // Lerp the skybox blend value
//////            Skybox_DualPanoramic.SetFloat("_Blend", Mathf.Lerp(startBlend, targetBlend, elapsedTime));

//////            // Lerp the directional light rotation
//////            float newRotationX = Mathf.Lerp(startRotationX, targetLightRotationX, elapsedTime);
//////            directionalLight.transform.rotation = Quaternion.Euler(newRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);

//////            yield return null;
//////        }

//////        // Ensure the final values are set precisely
//////        Skybox_DualPanoramic.SetFloat("_Blend", targetBlend);
//////        directionalLight.transform.rotation = Quaternion.Euler(targetLightRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);
//////    }

//////    // Set the scene to the default daylight state when Play mode starts
//////    private void SetDaylight()
//////    {
//////        // Set the skybox blend value to the day state
//////        Skybox_DualPanoramic.SetFloat("_Blend", dayBlendValue);

//////        // Set the directional light to the day rotation
//////        directionalLight.transform.rotation = Quaternion.Euler(dayLightRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);
//////    }

//////    private void OnDisable()
//////    {
//////        // Restore the initial blend value when the script is disabled or the game stops
////        Skybox_DualPanoramic.SetFloat("_Blend", initialBlendValue);
//////    }
//////}


////using UnityEngine;
////using System.Collections;

////public class SkyboxController : MonoBehaviour
////{
////    public GameObject unbloomCereus;
////    public Material Skybox_DualPanoramic; // Reference to the skybox material
////    public Light directionalLight; // Reference to the directional light

////    public float dayLightRotationX = 50f; // Rotation for day (morning)
////    public float nightLightRotationX = -20f; // Rotation for night

////    public float dayLightIntensity = 1f; // Intensity for day
////    public float nightLightIntensity = 0.05f; // Reduced intensity for night

////    [Range(0, 1)] public float dayBlendValue = 0f; // Blend value for day
////    [Range(0, 1)] public float nightBlendValue = 1f; // Blend value for night

////    public float transitionSpeed = 1f; // Speed of the transition

////    private float initialBlendValue; // Track initial blend value to restore during Play mode

////    public static bool IsNight { get; private set; } = false; // Flag to check if it's night time
////    public static bool SeedPlantedFlag { get; set; } = false; // Track if seed is planted
////    private bool seedPlantedAtNight = false; // Track if the seed was planted at night
////    private bool flowerBloomed = false; // Flag to check if the flower has bloomed

////    private PictureAlignNight pictureAlign;

////    //public GameObject seedUI;
////    private void Awake()
////    {
////        // Save the initial blend value when the script is loaded
////        initialBlendValue = Skybox_DualPanoramic.GetFloat("_Blend");
////        if (unbloomCereus != null)
////        {
////            unbloomCereus.SetActive(false);
////        }
////    }

////    private void Start()
////    {
////        // Reset the blend value, light rotation, and intensity to the default day state
////        SetDaylight();
////        if (unbloomCereus != null)
////        {
////            unbloomCereus.SetActive(false);
////        }

////        pictureAlign = FindObjectOfType<PictureAlignNight>();
////    }

////    private void Update()
////    {
////        // Press 'N' to switch to night skybox, rotation, and intensity
////        if (pictureAlign.alignCheck)
////        {
////            StartCoroutine(SmoothTransition(nightBlendValue, nightLightRotationX, nightLightIntensity));
////            IsNight = true;

////            if (SeedPlantedFlag && !flowerBloomed)
////            {
////                Debug.Log("Seed was planted during the day. Blooming flower immediately at night.");
////                BloomFlower();
////            }
////            // If the seed was planted during the day, bloom the flower immediately
////            //if (SeedPlantedFlag && !seedPlantedAtNight)
////            //{
////            //    Debug.Log("Seed was planted during the day. Blooming flower immediately at night.");
////            //    if (unbloomCereus != null)
////            //    {
////            //        unbloomCereus.SetActive(true); // Show flower
////            //    }
////            //}
////            //else if (SeedPlantedFlag && seedPlantedAtNight)
////            //{
////            //    Debug.Log("Seed was planted at night. Fading flower into existence.");
////            //    if (unbloomCereus != null)
////            //    {
////            //        //StartCoroutine(FadeInFlower(unbloomCereus));
////            //        unbloomCereus.SetActive(true);
////            //    }
////            //}

////            //if (unbloomCereus != null)
////            //{
////            //    // Toggle the visibility of the object when 'N' is pressed
////            //    unbloomCereus.SetActive(!unbloomCereus.activeSelf);
////            //}
////        }

////        // Press 'M' to switch to day skybox, rotation, and intensity
////        //if (Input.GetKeyDown(KeyCode.M))
////        //{
////        //    StartCoroutine(SmoothTransition(dayBlendValue, dayLightRotationX, dayLightIntensity));
////        //    IsNight = false;

////        //    // Reset the flower when transitioning to day
////        //    //if (unbloomCereus != null)
////        //    //{
////        //    //    unbloomCereus.SetActive(false);
////        //    //}

////        //}
////    }

////    private void BloomFlower()
////    {
////        // Make the flower appear and set the flag to true
////        if (unbloomCereus != null)
////        {
////            unbloomCereus.SetActive(true);
////            flowerBloomed = true; // Once bloomed, it should stay bloomed permanently
////        }
////    }

////    public void HandleSeedPlanted()
////    {
////        //SeedPlantedFlag = true;
////        //seedPlantedAtNight = IsNight; // Track if the seed was planted at night
////        SeedPlantedFlag = true;
////        seedPlantedAtNight = IsNight; // Track if the seed was planted at night

////        // If it's night, make the flower bloom immediately and keep it bloomed permanently
////        if (seedPlantedAtNight && !flowerBloomed)
////        {
////            Debug.Log("Seed planted at night. Making flower appear immediately and keeping it bloomed.");
////            BloomFlower();
////        }
////    }

////    // Coroutine to smoothly transition between day and night states
////    private IEnumerator SmoothTransition(float targetBlend, float targetLightRotationX, float targetLightIntensity)
////    {
////        float startBlend = Skybox_DualPanoramic.GetFloat("_Blend");
////        float startRotationX = directionalLight.transform.rotation.eulerAngles.x;
////        float startIntensity = directionalLight.intensity;
////        float elapsedTime = 0f;

////        while (elapsedTime < 1f)
////        {
////            elapsedTime += Time.deltaTime * transitionSpeed;

////            // Lerp the skybox blend value
////            Skybox_DualPanoramic.SetFloat("_Blend", Mathf.Lerp(startBlend, targetBlend, elapsedTime));

////            // Lerp the directional light rotation
////            float newRotationX = Mathf.Lerp(startRotationX, targetLightRotationX, elapsedTime);
////            directionalLight.transform.rotation = Quaternion.Euler(newRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);

////            // Lerp the directional light intensity
////            directionalLight.intensity = Mathf.Lerp(startIntensity, targetLightIntensity, elapsedTime);

////            yield return null;
////        }

////        // Ensure the final values are set precisely
////        Skybox_DualPanoramic.SetFloat("_Blend", targetBlend);
////        directionalLight.transform.rotation = Quaternion.Euler(targetLightRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);
////        directionalLight.intensity = targetLightIntensity;
////    }

////    // Set the scene to the default daylight state when Play mode starts
////    private void SetDaylight()
////    {
////        // Set the skybox blend value to the day state
////        Skybox_DualPanoramic.SetFloat("_Blend", dayBlendValue);

////        // Set the directional light to the day rotation and intensity
////        directionalLight.transform.rotation = Quaternion.Euler(dayLightRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);
////        directionalLight.intensity = dayLightIntensity;
////        IsNight = false;
////    }

////    //private IEnumerator FadeInFlower(GameObject flowerObject)
////    //{
////    //    if (flowerObject != null)
////    //    {
////    //        flowerObject.SetActive(true);
////    //    }
////    //    //flowerObject.SetActive(true);

////    //}

////    private void OnDisable()
////    {
////        // Restore the initial blend value when the script is disabled or the game stops
////        Skybox_DualPanoramic.SetFloat("_Blend", initialBlendValue);
////    }
////}

//using UnityEngine;
//using System.Collections;

//public class SkyboxController : MonoBehaviour
//{
//  public GameObject unbloomCereus;
//  public Material Skybox_DualPanoramic; // Reference to the skybox material
//  public Light directionalLight; // Reference to the directional light

//  public float dayLightRotationX = 50f; // Rotation for day (morning)
//  public float nightLightRotationX = -20f; // Rotation for night

//  public float dayLightIntensity = 1f; // Intensity for day
//  public float nightLightIntensity = 0.05f; // Reduced intensity for night

//  [Range(0, 1)] public float dayBlendValue = 0f; // Blend value for day
//  [Range(0, 1)] public float nightBlendValue = 1f; // Blend value for night

//  public float transitionSpeed = 1f; // Speed of the transition

//  private float initialBlendValue; // Track initial blend value to restore during Play mode

//  public static bool IsNight { get; private set; } = false; // Flag to check if it's night time
//  public static bool SeedPlantedFlag { get; set; } = false; // Track if seed is planted
//  private bool seedPlantedAtNight = false; // Track if the seed was planted at night
//  private bool flowerBloomed = false; // Flag to check if the flower has bloomed

//  [Header("References")]
//  public PictureAlignNight pictureAlign; // Public reference to PictureAlignNight

//  private Inventory inventoryScript;

//  // Track the associated item to remove
//  private IInventoryItem associatedItem;

//  private void Awake()
//  {
//    // Save the initial blend value when the script is loaded
//    initialBlendValue = Skybox_DualPanoramic.GetFloat("_Blend");

//    if (unbloomCereus != null)
//    {
//      unbloomCereus.SetActive(false);
//    }
//  }

//  private void Start()
//  {
//    inventoryScript = FindObjectOfType<Inventory>();
//    // Reset the blend value, light rotation, and intensity to the default day state
//    SetDaylight();

//    // Initialize the flower as hidden
//    if (unbloomCereus != null)
//    {
//      unbloomCereus.SetActive(false);
//    }
//  }

//  private void Update()
//  {
//    // Ensure pictureAlign is assigned before accessing alignCheck
//    if (pictureAlign != null && pictureAlign.isFrozen && !IsNight)
//    {
//      StartCoroutine(SmoothTransition(nightBlendValue, nightLightRotationX, nightLightIntensity));
//      IsNight = true;
//      IInventoryItem itemToUse = inventoryScript.GetItemByName("NightPainting");
//      Debug.Log("itemToUse" + itemToUse);
//      if (itemToUse != null)
//      {
//        Debug.Log("name" + itemToUse.Name);
//        inventoryScript.RemoveItem(itemToUse);
//      }

//      if (SeedPlantedFlag && !flowerBloomed)
//      {
//        Debug.Log("Seed was planted during the day. Blooming flower immediately at night.");
//        BloomFlower();
//      }
//    }
//  }

//  private void BloomFlower()
//  {
//    // Make the flower appear and set the flag to true
//    if (unbloomCereus != null)
//    {
//      unbloomCereus.SetActive(true);
//      flowerBloomed = true; // Once bloomed, it should stay bloomed permanently
//    }
//  }

//  public void HandleSeedPlanted()
//  {
//    SeedPlantedFlag = true;
//    seedPlantedAtNight = IsNight; // Track if the seed was planted at night

//    // If it's night, make the flower bloom immediately and keep it bloomed permanently
//    if (seedPlantedAtNight && !flowerBloomed)
//    {
//      Debug.Log("Seed planted at night. Making flower appear immediately and keeping it bloomed.");
//      BloomFlower();
//    }
//  }

//  // Coroutine to smoothly transition between day and night states
//  private IEnumerator SmoothTransition(float targetBlend, float targetLightRotationX, float targetLightIntensity)
//  {
//    float startBlend = Skybox_DualPanoramic.GetFloat("_Blend");
//    float startRotationX = directionalLight.transform.rotation.eulerAngles.x;
//    float startIntensity = directionalLight.intensity;
//    float elapsedTime = 0f;

//    while (elapsedTime < 1f)
//    {
//      elapsedTime += Time.deltaTime * transitionSpeed;

//      // Lerp the skybox blend value
//      Skybox_DualPanoramic.SetFloat("_Blend", Mathf.Lerp(startBlend, targetBlend, elapsedTime));

//      // Lerp the directional light rotation
//      float newRotationX = Mathf.Lerp(startRotationX, targetLightRotationX, elapsedTime);
//      directionalLight.transform.rotation = Quaternion.Euler(newRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);

//      // Lerp the directional light intensity
//      directionalLight.intensity = Mathf.Lerp(startIntensity, targetLightIntensity, elapsedTime);

//      yield return null;
//    }

//    // Ensure the final values are set precisely
//    Skybox_DualPanoramic.SetFloat("_Blend", targetBlend);
//    directionalLight.transform.rotation = Quaternion.Euler(targetLightRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);
//    directionalLight.intensity = targetLightIntensity;
//  }

//  // Set the scene to the default daylight state when Play mode starts
//  private void SetDaylight()
//  {
//    // Set the skybox blend value to the day state
//    Skybox_DualPanoramic.SetFloat("_Blend", dayBlendValue);

//    // Set the directional light to the day rotation and intensity
//    directionalLight.transform.rotation = Quaternion.Euler(dayLightRotationX, directionalLight.transform.rotation.eulerAngles.y, directionalLight.transform.rotation.eulerAngles.z);
//    directionalLight.intensity = dayLightIntensity;
//    IsNight = false;
//  }

//  private void OnDisable()
//  {
//    // Restore the initial blend value when the script is disabled or the game stops
//    Skybox_DualPanoramic.SetFloat("_Blend", initialBlendValue);
//  }
//}

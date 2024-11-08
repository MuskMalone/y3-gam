//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;
//public class PictureAlignTools : Entity
//{
//    public Transform player;            // Assign the player object
//    public Camera mainCamera;           // Assign the main camera for rotation checking
//    public string dataFileName = "ToolsPainting.txt";  // The name of the text file containing saved data

//    public float positionThreshold = 0.1f;  // Threshold for position alignment
//    public float rotationThreshold = 2f;    // Threshold for rotation alignment (in degrees)

//    private PlayerMotor playerMotor;        // Reference to the PlayerMotor script
//    private PlayerLook playerLook;          // Reference to the PlayerLook script
//    public bool isFrozen = false;           // Check if the player is frozen
//    public bool alignCheck = false;

//    public Entity Tools;

//    private Vector3 savedPosition;
//    private Quaternion savedRotation;
//    private Quaternion savedCameraRotation;

//    public TextAsset dataFile;

//    void Start()
//    {
//        // Initialize the movement and camera control components
//        playerMotor = player.GetComponent<PlayerMotor>();
//        playerLook = player.GetComponent<PlayerLook>();

//        if (playerMotor == null) Debug.LogError("PlayerMotor component not found!");
//        if (playerLook == null) Debug.LogError("PlayerLook component not found!");

//        // Load the saved data from the text file
//        //LoadDataFromTxt();
//        LoadDataFromTextAsset();

//        Tools.SetActive(false);
//    }

//    void Update()
//    {
//        // Perform alignment checks and freeze the player if aligned
//        if (IsAligned())
//        {
//            alignCheck = true;
//            Debug.Log("Player is aligned.");
//            //FreezePlayer();
//        }
//        else
//        {
//            alignCheck = false;
//        }

//        // Handle object states when the player is frozen
//        if (isFrozen)
//        {
//            Tools.SetActive(true);
//        }
//    }

//    bool IsAligned()
//    {

//        float positionDistance = Vector3.Distance(player.position, savedPosition);
//        Debug.Log("Distance from saved position: " + positionDistance);

//        float rotationDifference = Mathf.QuaternionAngle(player.rotation, savedRotation);
//        Debug.Log("Rotation difference from saved rotation: " + rotationDifference);

//        if (Vector3.Distance(player.position, savedPosition) > positionThreshold)
//        {
//            Debug.Log((Vector3.Distance(player.position, savedPosition)).ToString());
//            Debug.Log("Player position not aligned.");
//            return false;
//        }

//        // Check if the player's rotation is within the threshold of the saved rotation
//        if (Mathf.QuaternionAngle(player.rotation, savedRotation) > rotationThreshold)
//        {
//            Debug.Log("Player rotation not aligned.");
//            return false;
//        }

//        // Check if the main camera's rotation is within the threshold of the saved camera rotation
//        if (Mathf.QuaternionAngle(mainCamera.transform.rotation, savedCameraRotation) > rotationThreshold)
//        {
//            Debug.Log("Camera rotation not aligned.");
//            return false;
//        }

//        Debug.Log("Player is aligned.");
//        return true; // All checks passed, the player is aligned
//    }

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

//        isFrozen = true;
//    }

//    // Method to unfreeze the player if needed
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

//        isFrozen = false;
//    }

//    //void LoadDataFromTxt()
//    //{
//    //    // Build the path to the text file in the Assets folder
//    //    string dataPath = Path.Combine(Application.dataPath, dataFileName);

//    //    if (File.Exists(dataPath))
//    //    {
//    //        string[] lines = File.ReadAllLines(dataPath);

//    //        foreach (string line in lines)
//    //        {
//    //            if (line.StartsWith("Player Position:"))
//    //            {
//    //                savedPosition = ParseVector3(line.Replace("Player Position:", "").Trim());
//    //            }
//    //            else if (line.StartsWith("Player Rotation:"))
//    //            {
//    //                savedRotation = ParseQuaternion(line.Replace("Player Rotation:", "").Trim());
//    //            }
//    //            else if (line.StartsWith("Camera Rotation:"))
//    //            {
//    //                savedCameraRotation = ParseQuaternion(line.Replace("Camera Rotation:", "").Trim());
//    //            }
//    //        }

//    //        Debug.Log("Data loaded from text file:");
//    //        Debug.Log("Saved Position: " + savedPosition);
//    //        Debug.Log("Saved Rotation: " + savedRotation.eulerAngles);
//    //        Debug.Log("Saved Camera Rotation: " + savedCameraRotation.eulerAngles);
//    //    }
//    //    else
//    //    {
//    //        Debug.LogError("Data file not found: " + dataPath);
//    //    }
//    //}
//    void LoadDataFromTextAsset()
//    {
//        if (dataFile != null)
//        {
//            // Read lines from the TextAsset
//            string[] lines = dataFile.text.Split(new[] { '\r', '\n' }, System.StringSplitOptions.RemoveEmptyEntries);

//            foreach (string line in lines)
//            {
//                if (line.StartsWith("Player Position:"))
//                {
//                    savedPosition = ParseVector3(line.Replace("Player Position:", "").Trim());
//                }
//                else if (line.StartsWith("Player Rotation:"))
//                {
//                    savedRotation = ParseQuaternion(line.Replace("Player Rotation:", "").Trim());
//                }
//                else if (line.StartsWith("Camera Rotation:"))
//                {
//                    savedCameraRotation = ParseQuaternion(line.Replace("Camera Rotation:", "").Trim());
//                }
//            }

//            Debug.Log("Data loaded from text asset:");
//            Debug.Log("Saved Position: " + savedPosition);
//            Debug.Log("Saved Rotation: " + Mathf.QuatToEuler(savedRotation));
//            Debug.Log("Saved Camera Rotation: " + Mathf.QuatToEuler(savedCameraRotation));
//        }
//        else
//        {
//            Debug.LogError("No data file assigned to the TextAsset.");
//        }
//    }
//    // Helper method to parse a Vector3 from a string
//    Vector3 ParseVector3(string value)
//    {
//        value = value.Replace("(", "").Replace(")", ""); // Remove parentheses
//        string[] values = value.Split(',');

//        if (values.Length == 3)
//        {
//            float x = float.Parse(values[0].Trim());
//            float y = float.Parse(values[1].Trim());
//            float z = float.Parse(values[2].Trim());
//            return new Vector3(x, y, z);
//        }

//        return new Vector3();
//    }

//    // Helper method to parse a Quaternion from a string (as Euler angles)
//    Quaternion ParseQuaternion(string value)
//    {
//        Vector3 euler = ParseVector3(value);
//        return Mathf.EulertoQuat(euler);
//    }

//    public Quaternion GetSavedPlayerRotation()
//    {
//        return savedRotation;
//    }

//    // Method to retrieve the saved camera rotation
//    public Quaternion GetSavedCameraRotation()
//    {
//        return savedCameraRotation;
//    }
//}

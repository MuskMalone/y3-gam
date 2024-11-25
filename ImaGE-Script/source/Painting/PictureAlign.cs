using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
using System.Text.RegularExpressions;

public class PictureAlign : Entity
{
  public Entity player;            // Assign the player object
  public Entity mainCamera;           // Assign the main camera for rotation checking
  public string dataFileName = "CapturedData.txt";  // The name of the text file containing saved data

  public float positionThreshold = 0.5f;  // Threshold for position alignment
  public float rotationThreshold = 2f;    // Threshold for rotation alignment (in degrees)

  private PlayerMove playerMove;          // Reference to the PlayerLook script
  public bool isFrozen = false;           // Check if the player is frozen
  public bool alignCheck = false;

  public Entity initialObject;
  public Entity FinalObject;
  public string dataFilePath = "CapturedData.txt";


  private TextAsset dataFile;
  private Vector3 savedPosition;
  private Quaternion savedCameraRotation;

  static void TestQuaternionAngle(Quaternion q1, Quaternion q2, float expectedAngle)
  {
    // Call your function to calculate the angle
    float calculatedAngle = Mathf.QuaternionAngle(q1, q2);

    // Print results
    Console.WriteLine($"Expected: {expectedAngle}�, Calculated: {calculatedAngle}�");

    // Validate the result (considering floating-point precision issues)
    if (Mathf.Abs(calculatedAngle - expectedAngle) < 0.01f)
    {
      Console.WriteLine("Test Passed!");
    }
    else
    {
      Console.WriteLine("Test Failed!");
    }

    Console.WriteLine();
  }

  void Start()
  {
    // Initialize the movement and camera control components
    playerMove = player.FindObjectOfType<PlayerMove>();

    if (playerMove == null) Debug.LogError("PlayerMove component not found!");

    // Load the saved data from the text file
    //LoadDataFromTxt();

    LoadDataFromTextAsset();

    //initialObject.SetActive(true);
    //FinalObject.SetActive(false);

  }

  void LoadDataFromTextAsset()
  {
    if (dataFilePath != null)
    {
      dataFilePath = "../Assets/GameImg/" + dataFilePath;
      Console.WriteLine(dataFilePath);
      dataFile = new TextAsset(dataFilePath, dataFilePath);
    }


    if (dataFile != null)
    {
      // Read lines from the TextAsset
      string[] lines = dataFile.Text.Split(new[] { '\r', '\n' }, System.StringSplitOptions.RemoveEmptyEntries);

      foreach (string line in lines)
      {
        if (line.StartsWith("Player Position:"))
        {
          savedPosition = ParseVector3(line.Replace("Player Position:", "").Trim());
        }
        else if (line.StartsWith("Camera Rotation:"))
        {
          savedCameraRotation = ParseQuaternion(line.Replace("Camera Rotation:", "").Trim());
        }
      }

      //Debug.Log("Data loaded from text asset:");
      //Console.WriteLine("Saved Position: " + savedPosition);
      //Console.WriteLine("Saved Rotation: " + savedRotation);
      //Console.WriteLine("Saved Camera Rotation: " + savedCameraRotation);
      //Debug.Log("Saved Position: " + savedPosition);
      //Debug.Log("Saved Rotation: " + Mathf.QuatToEuler(savedRotation));
      //Debug.Log("Saved Camera Rotation: " + Mathf.QuatToEuler(savedCameraRotation));
    }
    else
    {
      Debug.LogError("No data file assigned to the TextAsset.");
    }
  }

  void Update()
  {
    // Perform alignment checks and freeze the player if aligned
    if (IsAligned())
    {
      alignCheck = true;
      Debug.Log("Player is aligned.");
      //FreezePlayer();
    }
    else
    {
      alignCheck = false;
    }

    // Handle object states when the player is frozen
    if (isFrozen)
    {
      //initialObject.SetActive(false);
      //FinalObject.SetActive(true);
    }
  }

  bool IsAligned()
  {

    float positionDistance = Vector3.Distance(player.GetComponent<Transform>().worldPosition, savedPosition);


    if (positionDistance > positionThreshold)
    {
      Debug.Log(positionDistance.ToString());
      Debug.Log("Player position not aligned.");
      Debug.Log("Distance from saved position: " + positionDistance);
      return false;
    }

    // Check if the main camera's rotation is within the threshold of the saved camera rotation
    if (Mathf.QuaternionAngle(mainCamera.GetComponent<Transform>().worldRotation, savedCameraRotation) > rotationThreshold)
    {
      Debug.Log("Camera rotation not aligned." + Mathf.QuaternionAngle(mainCamera.GetComponent<Transform>().rotation, savedCameraRotation));
      return false;
    }




    Debug.Log("Player is aligned.");
    return true; // All checks passed, the player is aligned
  }

  public void FreezePlayer()
  {
    if (playerMove != null)
    {
      playerMove.canMove = false;  // Freeze player movement
      Debug.Log("Player movement frozen.");
    }

    if (playerMove != null)
    {
      playerMove.canLook = false;  // Freeze camera movement
      Debug.Log("Player camera look frozen.");
    }

    isFrozen = true;
  }

  // Method to unfreeze the player if needed
  public void UnfreezePlayer()
  {
    if (playerMove != null)
    {
      playerMove.canMove = true;  // Unfreeze player movement
    }

    if (playerMove != null)
    {
      playerMove.canLook = true;  // Unfreeze camera movement
    }

    isFrozen = false;
  }

  // Helper method to parse a Vector3 from a string
  Vector3 ParseVector3(string value)
  {
    value = value.Replace("(", "").Replace(")", ""); // Remove parentheses
    value = value.Replace("<", "");
    value = value.Replace(">", "");
    string[] values = value.Split(',');

    if (values.Length == 3)
    {
      float x = float.Parse(values[0].Trim());
      float y = float.Parse(values[1].Trim());
      float z = float.Parse(values[2].Trim());
      return new Vector3(x, y, z);
    }

    return new Vector3(0);
  }

  Quaternion ParseQuaternion(string value)
  {
    Console.WriteLine("B4: " + value);
    value = value.Replace("(", "").Replace(")", ""); // Remove parentheses
    value = value.Replace("X", "");
    value = value.Replace("Y", "");
    value = value.Replace("Z", "");
    value = value.Replace("W", "");
    value = value.Replace("{", "");
    value = value.Replace("}", "");
    Console.WriteLine("AFT: " + value);
    string[] values = value.Split(':');

    foreach (string s in values)
    {
      Console.WriteLine(s);
    }

    if (values.Length == 5)
    {
      float x = float.Parse(values[1].Trim());
      float y = float.Parse(values[2].Trim());
      float z = float.Parse(values[3].Trim());
      float w = float.Parse(values[4].Trim());
      return new Quaternion(x, y, z, w);
    }

    return new Quaternion();
  }

  // Method to retrieve the saved camera rotation
  public Quaternion GetSavedCameraRotation()
  {
    return savedCameraRotation;
  }
}

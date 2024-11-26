using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
public class CameraCapture : Entity
{
  //public Camera mainCamera;          // Assign the main camera
  public Entity player;           // Assign the player object
  public int imageWidth = 900;       // Width of the capture
  public int imageHeight = 900;      // Height of the capture
  KeyCode captureKey = KeyCode.L;  // Key to capture the image
  public string PictureName = "CapturedImage";

  // Variables to store the player's position, rotation, and main camera's rotation
  private Vector3 savedPlayerPosition;
  private Quaternion savedCameraRotation;

  public bool imageCaptured = false;  // Check if the image and data have been captured

  void Start()
  {
  }

  void Update()
  {
    // Capture image and store data when the capture key is pressed
    if (Input.GetKeyDown(captureKey))
    {
      CaptureImageAndStoreData();
    }
  }

  void CaptureImageAndStoreData()
  {
     InternalCalls.TakeScreenShot(PictureName, imageWidth, imageHeight);

    // Store the player's position, rotation, and the main camera's rotation
    savedPlayerPosition = player.GetComponent<Transform>().worldPosition;
    savedCameraRotation = InternalCalls.GetMainCameraRotation(FindEntityByTag("MainCamera").mEntityID);

    Debug.Log("Player Position: " + savedPlayerPosition);
    Debug.Log("Camera Rotation: " + savedCameraRotation);

    // Set imageCaptured to true, indicating that the data is stored
    imageCaptured = true;

    // Save the captured data to a text file
    ExportDataToTxt();
  }

  // Method to export data as a .txt file in the assets folder
  void ExportDataToTxt()
  {
    // Create the path for the text file in the assets folder
    string dataPath = ("../Assets/GameImg/" + PictureName + ".txt");

    // Create the content to be written to the file
    string content = "Captured Data:\n";
    content += $"Player Position: {savedPlayerPosition}\n";
    content += $"Camera Rotation: {savedCameraRotation}\n";

    // Write the content to the text file
    File.WriteAllText(dataPath, content);

    Debug.Log($"Data exported to: {dataPath}");
  }

  // Public methods to retrieve the saved data for alignment checks in another script
  public Vector3 GetSavedPlayerPosition()
  {
    return savedPlayerPosition;
  }

  public Quaternion GetSavedCameraRotation()
  {
    return savedCameraRotation;
  }

  public bool IsImageCaptured()
  {
    return imageCaptured;
  }
}

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
  private Vector3 savedCameraEuler;

  // whether the painting needs to be unlocked (by another script) before allowing alignment
  public bool shouldLock = false;
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
    // need to handle taking screenshot at diff resolutions 
    // base resolution is 1920x1080 image was 700x700
    // if we were to take screenshot/snapshot on a different screen resolution, we need to scale the image accordingly to ensure ewe capture all the info
    int newWidth  =  (int)((float)Input.screenWidth * (float)imageWidth / 1920.0f);     // ensure that the width of the screenshot is always 700/1920 of the screen width
    int newHeight = (int)((float)Input.screenHeight * (float)imageHeight / 1080.0f);    // ensure that the height of the screenshot is always 700/1080 of the screen height
    InternalCalls.TakeScreenShot(PictureName, newWidth, newHeight);

    // Store the player's position, rotation, and the main camera's rotation
    savedPlayerPosition = player.GetComponent<Transform>().worldPosition;
    savedCameraRotation = InternalCalls.GetMainCameraRotation(FindEntityByTag("MainCamera").mEntityID);
    PlayerMove playerM = FindObjectOfType<PlayerMove>();
    savedCameraEuler = new Vector3(playerM.GetRotation(),0);

    Debug.Log("Player Position: " + savedPlayerPosition);
    Debug.Log("Camera Rotation: " + savedCameraRotation);
    Debug.Log("Camera Euler: " + savedCameraEuler);

    // Set imageCaptured to true, indicating that the data is stored
    imageCaptured = true;

    // Save the captured data to a text file
    ExportDataToTxt(newWidth,newHeight);
  }

  // Method to export data as a .txt file in the assets folder
  void ExportDataToTxt(int w, int h)
  {
    // Create the path for the text file in the assets folder
    string dataPath = ("../Assets/GameImg/" + PictureName + ".txt");

    // Create the content to be written to the file
    string content = "Captured Data:\n";
    content += $"Player Position: {savedPlayerPosition}\n";
    content += $"Camera Rotation: {savedCameraRotation}\n";
    content += $"Camera Euler:    {savedCameraEuler}\n";
    content += $"shouldLock:      {shouldLock}\n";
    content += $"Image Width:     {w}\n";   // We need to pass the width and height of image in. we will use these info to scale the image when we display on different screens
    content += $"Image Height:    {h}\n";
    content += $"ScreenWidth:     {Input.screenWidth}\n";   // We need to pass the width and height of screen in. we will use these info to scale the image when we display on different screens
    content += $"ScreenHeight:    {Input.screenHeight}\n";

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

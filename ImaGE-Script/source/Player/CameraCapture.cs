using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Image.Utils;
public class CameraCapture : Entity
{
    public Camera mainCamera;          // Assign the main camera
    public Camera secondaryCamera;     // Assign the secondary camera (this will follow the main camera)
    public Transform player;           // Assign the player object
    public int imageWidth = 256;       // Width of the capture
    public int imageHeight = 256;      // Height of the capture
    public KeyCode captureKey = KeyCode.P;  // Key to capture the image

    // Variables to store the player's position, rotation, and main camera's rotation
    private Vector3 savedPlayerPosition;
    private Quaternion savedPlayerRotation;
    private Quaternion savedCameraRotation;

    public bool imageCaptured = false;  // Check if the image and data have been captured

    void Start()
    {
        // Ensure the secondary camera is disabled at the start
        if (secondaryCamera != null)
        {
            secondaryCamera.enabled = false;
        }
    }

    void Update()
    {
        // Make the secondary camera follow the main camera
        if (secondaryCamera != null && mainCamera != null)
        {
            secondaryCamera.transform.position = mainCamera.transform.position;
            secondaryCamera.transform.rotation = mainCamera.transform.rotation;
        }

        // Capture image and store data when the capture key is pressed
        if (Input.GetKeyDown(captureKey))
        {
            StartCoroutine(CaptureImageAndStoreData());
        }
    }

    IEnumerator CaptureImageAndStoreData()
    {
        // Enable the secondary camera just for the capture
        secondaryCamera.enabled = true;

        // Wait for the end of the frame to ensure everything is rendered
        yield return new WaitForEndOfFrame();

        // Set up a RenderTexture for the secondary camera
        RenderTexture renderTexture = new RenderTexture(imageWidth, imageHeight, 24);
        secondaryCamera.targetTexture = renderTexture;
        Texture2D screenShot = new Texture2D(imageWidth, imageHeight, TextureFormat.RGB24, false);

        // Render the camera's view to the RenderTexture
        secondaryCamera.Render();

        // Read the pixels from the RenderTexture into a Texture2D
        RenderTexture.active = renderTexture;
        screenShot.ReadPixels(new Rect(0, 0, imageWidth, imageHeight), 0, 0);
        screenShot.Apply();

        // Reset the camera's target texture and clean up
        secondaryCamera.targetTexture = null;
        RenderTexture.active = null;
        Destroy(renderTexture);

        // Convert the captured image to PNG format
        byte[] bytes = screenShot.EncodeToPNG();
        string imagePath = Path.Combine(Application.dataPath, "CapturedImage.png");

        // Write the PNG to the assets folder
        File.WriteAllBytes(imagePath, bytes);

        Debug.Log($"Screenshot saved to: {imagePath}");

        // Store the player's position, rotation, and the main camera's rotation
        savedPlayerPosition = player.position;
        savedPlayerRotation = player.rotation;
        savedCameraRotation = mainCamera.transform.rotation;

        Debug.Log("Player Position: " + savedPlayerPosition);
        Debug.Log("Player Rotation: " + savedPlayerRotation.eulerAngles);
        Debug.Log("Camera Rotation: " + savedCameraRotation.eulerAngles);

        // Set imageCaptured to true, indicating that the data is stored
        imageCaptured = true;

        // Save the captured data to a text file
        ExportDataToTxt();

        // Disable the secondary camera after capturing the image
        secondaryCamera.enabled = false;
    }

    // Method to export data as a .txt file in the assets folder
    void ExportDataToTxt()
    {
        // Create the path for the text file in the assets folder
        string dataPath = Path.Combine(Application.dataPath, "CapturedData.txt");

        // Create the content to be written to the file
        string content = "Captured Data:\n";
        content += $"Player Position: {savedPlayerPosition}\n";
        content += $"Player Rotation: {savedPlayerRotation.eulerAngles}\n";
        content += $"Camera Rotation: {savedCameraRotation.eulerAngles}\n";

        // Write the content to the text file
        File.WriteAllText(dataPath, content);

        Debug.Log($"Data exported to: {dataPath}");
    }

    // Public methods to retrieve the saved data for alignment checks in another script
    public Vector3 GetSavedPlayerPosition()
    {
        return savedPlayerPosition;
    }

    public Quaternion GetSavedPlayerRotation()
    {
        return savedPlayerRotation;
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

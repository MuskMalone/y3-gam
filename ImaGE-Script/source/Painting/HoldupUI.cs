using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
using System.Data;
public class HoldupUI : Entity
{
  //Data on how to display the Image on the screen
  public bool isBigPaintingActive = false;
  public Vector3 bigPicPos = new Vector3(0, 0.17f, 0);
  public Vector3 bigPicScale = new Vector3(12.980f, 13.25f, 12.980f);
  public Vector3 smallPicPos = new Vector3(10,0, 0);
  public Vector3 smallPicScale = new Vector3(6, 6, 6);
  private int ImageWidth = 700;   // Width of the image
  private int ImageHeight = 700;  // Height of the image
  private int OgScreenWidth = 1920; //The width of the screen on which the image was taken.
  private int OgScreenHeight = 1080; //The height of the screen on which the image was taken.

  // AudioManager audioManager;
  private Inventory inventoryScript;
  private TutorialLevelInventory tutorialInventoryScript;
  private Level2Inventory level2InventoryScript;
  private Level3Inventory level3InventoryScript;

  // Track the associated item to remove
  private IInventoryItem associatedItem;

  //Alignment data for the Image
  public string dataFilePath = "CapturedData.txt";
  private TextAsset dataFile;
  private Vector3 savedPosition;
  private Quaternion savedCameraRotation;
  private Vector3 saveCamEuler;
  private bool shouldLock;
  private PictureAlign pictureAlignscript;  

  private void Awake()
  {
    //audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
  }

  void Start()
  {
    inventoryScript = FindObjectOfType<Inventory>();
    tutorialInventoryScript = FindObjectOfType<TutorialLevelInventory>();
    level2InventoryScript = FindObjectOfType<Level2Inventory>();
    level3InventoryScript = FindObjectOfType<Level3Inventory>();
    pictureAlignscript = FindObjectOfType<PictureAlign>();
    //if (pictureAlignscript != null)
    //  Console.WriteLine(pictureAlignscript.GetComponent<Tag>().tag);
    //else
    //  Console.WriteLine("No pic align");

    LoadDataFromTextAsset();
    if (isBigPaintingActive)
    {
      // ensure that the width of the screenshot is always 700/1920 of the screen width
      // ensure that the height of the screenshot is always 700/1080 of the screen height
      Vector3 actualScale = bigPicScale;
      actualScale.X = (float)ImageWidth / (float)OgScreenWidth * (float)Input.screenWidth;

      float Scaler = (  (((float)ImageHeight / (float)Input.screenHeight) / (700.0f / 1080.0f)) *
                        (((float)Input.screenHeight / (float)Input.screenWidth) / (1080.0f / 1920.0f))
                        );
      actualScale.Y = Scaler * bigPicScale.Y * ((float)ImageHeight / (float)Input.screenHeight) / ((float)ImageHeight/(float)OgScreenHeight);
      Vector3 actualPos = bigPicPos;
      actualPos.Y = Scaler * bigPicPos.Y;

      GetComponent<Transform>().position = actualPos;
      GetComponent<Transform>().scale = actualScale;
    }
    else
    { 
      GetComponent<Transform>().position = smallPicPos;
      GetComponent<Transform>().scale = smallPicScale;
    }
  }


  void Update()
  {
    //if (imageDisintegrating.canDestroy)
    //{
    //  pictureAlignscript.UnfreezePlayer();
    //  pictureAlignscript.isFrozen = false;

    //  IInventoryItem itemToUse = inventoryScript.GetItemByName("Pit Painting");
    //  Debug.Log("itemToUse" + itemToUse);
    //  if (itemToUse != null)
    //  {
    //    Debug.Log("name" + itemToUse.Name);
    //    inventoryScript.RemoveItem(itemToUse);
    //  }

    //  Destroy(Entity);
    //}

    if (InternalCalls.IsKeyTriggered(KeyCode.G))
      SetActive(true);
      

    if (!pictureAlignscript.isFrozen && Input.GetMouseButtonTriggered(1))
    {
      isBigPaintingActive = !isBigPaintingActive;
      pictureAlignscript.SetBorder(isBigPaintingActive);
  
      if (isBigPaintingActive)
      {
        float Scaler = (
                               (((float)ImageHeight / (float)Input.screenHeight) / (700.0f / 1080.0f)) *
                               (((float)Input.screenHeight / (float)Input.screenWidth) / (1080.0f / 1920.0f))
                             );
        Vector3 actualScale = bigPicScale;
         actualScale.Y = Scaler * bigPicScale.Y * ((float)ImageHeight / (float)Input.screenHeight) / ((float)ImageHeight/(float)OgScreenHeight);
        Vector3 actualPos = bigPicPos;
        actualPos.Y = Scaler * bigPicPos.Y;

        GetComponent<Transform>().position = actualPos;
        GetComponent<Transform>().scale = actualScale;
      }
      else
      {

        GetComponent<Transform>().position = smallPicPos;
        GetComponent<Transform>().scale = smallPicScale;
      }
    }

    //// Check if the player can freeze
    //if (pictureAlignscript.alignCheck && Input.GetMouseButtonDown(0) && !pictureAlignscript.isFrozen)
    //{
    //  audioManager.PlaySFX(audioManager.paintingMatch);
    //  pictureAlignscript.isFrozen = true;
    //  pictureAlignscript.FreezePlayer();
    //}
  }

  public void SetAlginUI(string s, IInventoryItem i)
  {
    associatedItem = i;
    if (pictureAlignscript != null)
    {
      if (shouldLock)
      {
        pictureAlignscript.preventAlignment = true;
      }
      else
      {
        pictureAlignscript.preventAlignment = false;
      }

      pictureAlignscript.SetActive(true);
      pictureAlignscript.SetTarget(savedPosition, savedCameraRotation, saveCamEuler,s,this);
      pictureAlignscript.SetBorder(isBigPaintingActive);
    }
    else
      Debug.Log("Picture Align is null");

    if (isBigPaintingActive)
    {
      float Scaler = (
                             (((float)ImageHeight / (float)Input.screenHeight) / (700.0f / 1080.0f)) *
                             (((float)Input.screenHeight / (float)Input.screenWidth) / (1080.0f / 1920.0f))
                           );
      Vector3 actualScale = bigPicScale;
      actualScale.Y = Scaler * bigPicScale.Y * ((float)ImageHeight / (float)Input.screenHeight) / ((float)ImageHeight/(float)OgScreenHeight);
      Vector3 actualPos = bigPicPos;
      actualPos.Y = Scaler * bigPicPos.Y;

      GetComponent<Transform>().position = actualPos;
        GetComponent<Transform>().scale = actualScale;
    }
    else
    {
      Console.WriteLine(GetComponent<Tag>().tag + " set small");
      GetComponent<Transform>().position = smallPicPos;
      GetComponent<Transform>().scale = smallPicScale;
    }
  }

  ///Functions to load Picture data
  
  //private float GetScaler()
  //{
  //   // we need to e
  //}


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
      // default to unlocked (to account for older dataFiles)
      shouldLock = false;

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
        else if (line.StartsWith("Camera Euler:"))
        {
          saveCamEuler = ParseVector3(line.Replace("Camera Euler:", "").Trim());
        }
        else if (line.StartsWith("shouldLock:"))
        {
          shouldLock = bool.Parse(line.Replace("shouldLock:", "").Trim());
        }
        else if (line.StartsWith("Image Width:"))
        {
          ImageWidth = int.Parse(line.Replace("Image Width:", "").Trim());
        }
        else if (line.StartsWith("Image Height:"))
        {
          ImageHeight = int.Parse(line.Replace("Image Height:", "").Trim());
        }
        else if (line.StartsWith("ScreenWidth:"))
        {
          OgScreenWidth = int.Parse(line.Replace("ScreenWidth:", "").Trim());
        }
        else if (line.StartsWith("ScreenHeight:"))
        {
          OgScreenHeight = int.Parse(line.Replace("ScreenHeight:", "").Trim());
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

  public void RemoveItself()
  {
    inventoryScript.RemoveItem(associatedItem);
    
  }

    public void TutorialRemoveItself()
    {
        tutorialInventoryScript.RemoveItem(associatedItem);
    }

    public void Level2RemoveItself()
    {
        level2InventoryScript.RemoveItem(associatedItem);
    }

    public void Level3RemoveItself()
    {
        level3InventoryScript.RemoveItem(associatedItem);
    }
}

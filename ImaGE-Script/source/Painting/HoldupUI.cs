using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;
public class HoldupUI : Entity
{
  //Data on how to display the Image on the screen
  private bool isBigPaintingActive = false;
  public Vector3 bigPicPos = new Vector3(0, 0, 0);
  public Vector3 bigPicScale = new Vector3(16.646f, 16.646f, 16.646f);
  public Vector3 smallPicPos = new Vector3(10,0, 0);
  public Vector3 smallPicScale = new Vector3(6, 6, 6);

  // AudioManager audioManager;
  private Inventory inventoryScript;

  // Track the associated item to remove
  private IInventoryItem associatedItem;

  //Alignment data for the Image
  public string dataFilePath = "CapturedData.txt";
  private TextAsset dataFile;
  private Vector3 savedPosition;
  private Quaternion savedCameraRotation;
  private PictureAlign pictureAlignscript;  

  private void Awake()
  {
    //audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
  }

  void Start()
  {
    inventoryScript = FindObjectOfType<Inventory>();
    pictureAlignscript = FindObjectOfType<PictureAlign>();
    if (pictureAlignscript != null)
      Console.WriteLine(pictureAlignscript.GetComponent<Tag>().tag);
    else
      Console.WriteLine("No pic align");

    LoadDataFromTextAsset();
    if (isBigPaintingActive)
    {
      GetComponent<Transform>().position = bigPicPos;
      GetComponent<Transform>().scale = bigPicScale;
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

    if(InternalCalls.IsKeyTriggered(KeyCode.Y))
      SetActive(true);

    if (!pictureAlignscript.isFrozen && Input.GetMouseButtonTriggered(1))
    {
      isBigPaintingActive = !isBigPaintingActive;
  
      if (isBigPaintingActive)
      {
        Console.WriteLine("Switch to Big Pic");
        GetComponent<Transform>().position = bigPicPos;
        GetComponent<Transform>().scale = bigPicScale;
      }
      else
      {
        Console.WriteLine("Switch to Small Pic");
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

  public void SetAlginUI()
  {
    if (pictureAlignscript != null)
    {
      if (isBigPaintingActive)
      {
        pictureAlignscript.SetActive(true);
        //pictureAlignscript.
      }
        
      else
        pictureAlignscript.SetActive(false);
    }
    else
      Debug.Log("Picture Align is null");
  }

  ///Functions to load Picture data

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
}

//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;

//using IGE.Utils;
//using System.Numerics;

//public class SelectionManager : Entity
//{
//  public static SelectionManager Instance { get; set; }

//  public bool onTarget;
//  public Entity Cursor_text;
//  //TextMeshProUGUI interaction_text;
//  public Vector3 textOffset = new Vector3(0, -80, 0);

//  private CleanWindows cleanWindows;

//  private void Start()
//  {
//    onTarget = false;
//    //interaction_text = Cursor_text.GetComponent<TextMeshProUGUI>();
//    cleanWindows = FindObjectOfType<CleanWindows>();
//    if (cleanWindows == null) Debug.LogError("cleanWindows component not found!");
//  }

//  private void Awake()
//  {
//    if (Instance != null && Instance != this)
//    {
//      Destroy(this.mEntityID);
//    }
//    else
//    {
//      Instance = this;
//    }
//  }

//  void Update()
//  {

//    if (Cursor_text != null)
//    {
//      Cursor_text.GetComponent<Transform>().position = Input.mousePosition + textOffset;
//    }

//    Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
//    RaycastHit hit;
//    if (Physics.Raycast(ray, out hit))
//    {
//      var selectionTransform = hit.transform;
//      InteractableObject interactable = selectionTransform.GetComponent<InteractableObject>();
//      NoteInteraction noteInteractable = selectionTransform.GetComponent<NoteInteraction>();
//      watercloth waterClothInteractable = selectionTransform.GetComponent<watercloth>();
//      CleanWindows cleanWindowsInteractable = selectionTransform.GetComponent<CleanWindows>();

//      if (interactable && interactable.playerInRange)
//      {
//        onTarget = true;

//        interaction_text.text = interactable.GetItemName();
//        Cursor_text.SetActive(true);
//      }
//      else if (noteInteractable && noteInteractable.playerInRange)
//      {
//        onTarget = true;
//        interaction_text.text = noteInteractable.GetItemName();
//        Cursor_text.SetActive(true);
//      }
//      else if (waterClothInteractable && waterClothInteractable.playerInRange)
//      {
//        onTarget = true;
//        interaction_text.text = waterClothInteractable.GetItemName();
//        Cursor_text.SetActive(true);
//      }
//      else if (cleanWindowsInteractable && cleanWindowsInteractable.playerInRange)
//      {
//        onTarget = true;
//        if (cleanWindowsInteractable.hasCleanedWindow)
//        {
//          interaction_text.text = cleanWindowsInteractable.GetAltItemName();
//        }
//        else
//        {
//          interaction_text.text = cleanWindowsInteractable.GetItemName();
//        }
//        Cursor_text.SetActive(true);
//      }
//      else
//      {
//        onTarget = false;
//        Cursor_text.SetActive(false);
//      }

//    }
//    else
//    {
//      onTarget = false;
//      Cursor_text.SetActive(false);
//    }
//  }
//}
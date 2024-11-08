//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;
//public class HoldupUINight : Entity
//{
//    public Entity bigPainting;
//    public Entity smallPainting;

//    private PictureAlignNight pictureAlignscript;
//    private ImageDisintegratingNight imageDisintegrating;
//    private bool isBigPaintingActive = false;

//    AudioManager audioManager;
//    private Inventory inventoryScript;

//    // Track the associated item to remove
//    private IInventoryItem associatedItem;

//    private void Awake()
//    {
//        audioManager = Entity.FindEntityWithTag("Audio").GetComponent<AudioManager>();
//    }

//    void Start()
//    {
//        inventoryScript = FindObjectOfType<Inventory>();
//        pictureAlignscript = FindObjectOfType<PictureAlignNight>();
//        imageDisintegrating = FindObjectOfType<ImageDisintegratingNight>();
//        bigPainting.SetActive(false);
//        smallPainting.SetActive(true);
//    }


//    void Update()
//    {
//        if (imageDisintegrating.canDestroy)
//        {
//            pictureAlignscript.UnfreezePlayer();
//            pictureAlignscript.isFrozen = false;

//            IInventoryItem itemToUse = inventoryScript.GetItemByName("Night Painting");
//            Debug.Log("itemToUse" + itemToUse);
//            if (itemToUse != null)
//            {
//                Debug.Log("name" + itemToUse.Name);
//                inventoryScript.RemoveItem(itemToUse);
//            }

//            Destroy(Entity);
//        }

//        if (!pictureAlignscript.isFrozen && Input.GetMouseButtonDown(1))
//        {
//            isBigPaintingActive = !isBigPaintingActive;

//            if (isBigPaintingActive)
//            {
//                bigPainting.SetActive(true);
//                smallPainting.SetActive(false);
//            }
//            else
//            {
//                bigPainting.SetActive(false);
//                smallPainting.SetActive(true);
//            }
//        }

//        // Check if the player can freeze
//        if (pictureAlignscript.alignCheck && Input.GetMouseButtonDown(0) && !pictureAlignscript.isFrozen)
//        {
//            audioManager.PlaySFX(audioManager.paintingMatch);
//            pictureAlignscript.isFrozen = true;
//            pictureAlignscript.FreezePlayer();
//        }
//    }
//}

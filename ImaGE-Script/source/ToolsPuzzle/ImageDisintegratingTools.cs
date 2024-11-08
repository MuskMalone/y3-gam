//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;
//public class ImageDisintegratingTool : Entity
//{
//    private PictureAlignTools pictureAlign;
//    private Image uiImage;
//    public float fadeSpeed = 0.5f;
//    private float targetAlpha = 1f;
//    public float fadeThreshold = 0.01f;
//    public bool canDestroy = false;

//    // Start is called before the first frame update
//    void Start()
//    {

//        pictureAlign = FindObjectOfType<PictureAlignTools>();


//        uiImage = GetComponent<Image>();


//        if (uiImage != null)
//        {
//            Color color = uiImage.color;
//            color.a = 1f;
//            uiImage.color = color;
//        }
//        else
//        {
//            Debug.LogError("UI Image component is missing.");

//        }
//    }

//    // Update is called once per frame
//    void Update()
//    {
//        if (pictureAlign != null)
//        {

//            if (pictureAlign.isFrozen)
//            {

//                targetAlpha = 0f;
//            }
//            else
//            {

//                targetAlpha = 1f;
//            }


//            FadeImage();
//        }
//        else
//        {
//            Destroy(Entity);
//        }
//    }

//    void FadeImage()
//    {
//        if (uiImage != null)
//        {
//            Color color = uiImage.color;

//            color.a = Mathf.Lerp(color.a, targetAlpha, fadeSpeed * Time.deltaTime);
//            uiImage.color = color;

//            if (Mathf.Abs(color.a - targetAlpha) < fadeThreshold && targetAlpha == 0f)
//            {

//                canDestroy = true;

//            }
//        }
//    }
//}
////using UnityEngine;
////using UnityEngine.UI;

////public class ImageDisintegratingNight : Entity
////{
////    private PictureAlign pictureAlign;
////    private Image uiImage;
////    public float fadeSpeed = 0.5f;
////    private float targetAlpha = 1f;
////    public float fadeThreshold = 0.01f;
////    public bool canDestroy = false;

////    void Start()
////    {
////        // Find PictureAlign in the scene
////        pictureAlign = FindObjectOfType<PictureAlign>();

////        // Get the Image component for fading
////        uiImage = GetComponent<Image>();

////        if (uiImage != null)
////        {
////            Color color = uiImage.color;
////            color.a = 1f; // Ensure image starts fully visible
////            uiImage.color = color;
////        }
////        else
////        {
////            Debug.LogError("UI Image component is missing.");
////        }
////    }

////    void Update()
////    {
////        // Remove the automatic destruction if PictureAlign is null
////        // if (pictureAlign == null) Destroy(Entity);  // This is no longer needed

////        if (pictureAlign != null && pictureAlign.isFrozen)
////        {
////            targetAlpha = 0f;  // Start fading out when frozen
////        }
////        else
////        {
////            targetAlpha = 1f;  // Keep image fully visible otherwise
////        }

////        // Perform the fading effect
////        FadeImage();
////    }

////    void FadeImage()
////    {
////        if (uiImage != null)
////        {
////            Color color = uiImage.color;

////            // Lerp the alpha value towards the target alpha
////            color.a = Mathf.Lerp(color.a, targetAlpha, fadeSpeed * Time.deltaTime);
////            uiImage.color = color;

////            // Check if the image has fully faded out
////            if (Mathf.Abs(color.a - targetAlpha) < fadeThreshold && targetAlpha == 0f)
////            {
////                canDestroy = true;  // Set the flag for destruction if fully faded
////            }
////        }
////    }
////}

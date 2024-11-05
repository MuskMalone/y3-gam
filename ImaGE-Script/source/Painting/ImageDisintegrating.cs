using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Image.Utils;

public class ImageDisintegrating : Entity
{
    private PictureAlign pictureAlign;
    private Image uiImage; 
    public float fadeSpeed = 0.5f; 
    private float targetAlpha = 1f; 
    public float fadeThreshold = 0.01f; 
    public bool canDestroy = false;

    // Start is called before the first frame update
    void Start()
    {

        pictureAlign = FindObjectOfType<PictureAlign>();


        uiImage = GetComponent<Image>();


        if (uiImage != null)
        {
            Color color = uiImage.color;
            color.a = 1f; 
            uiImage.color = color;
        }
        else
        {
            Debug.LogError("UI Image component is missing.");
            
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (pictureAlign != null)
        {

            if (pictureAlign.isFrozen)
            {
   
                targetAlpha = 0f;
            }
            else
            {

                targetAlpha = 1f;
            }


            FadeImage();
        }
        else
        {
            Destroy(Entity);
        }
    }

    void FadeImage()
    {
        if (uiImage != null)
        {
            Color color = uiImage.color;

            color.a = Mathf.Lerp(color.a, targetAlpha, fadeSpeed * Time.deltaTime);
            uiImage.color = color;

            if (Mathf.Abs(color.a - targetAlpha) < fadeThreshold && targetAlpha == 0f)
            {

                canDestroy = true;
                
            }
        }
    }
}

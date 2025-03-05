/******************************************************************************/
/*!
\par        Image Engine
\file       .cs

\author     
\date       

\brief      


Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.Remoting.Metadata.W3cXsd2001;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;

public class InventoryTutorial : Entity
{
    // Start is called before the first frame update
    private Image tutorialImage;
    private WASDTutorial wasdTutorial;

    public float fadeSpeed = 2f;
    private float currentAlpha = 0f;

    private bool isFadingIn = false;
    private bool isFadingOut = false;
    private bool isVisible = false;
    public bool finish = false;

    private float timer = 0f;
    public float duration = 4f;

    public bool wasBigPaintingActive = false;

    void Start()
    {
        tutorialImage = GetComponent<Image>();
        wasdTutorial = FindObjectOfType<WASDTutorial>();

        if (tutorialImage != null)
        {
            Color color = tutorialImage.color;
            color.a = 0f;
            tutorialImage.color = color;
        }
        else
        {
            Debug.LogError("Tutorial Image component is missing.");

        }
    }

    // Update is called once per frame
    void Update()
    {
        if (wasdTutorial.finish && !isFadingIn && !isVisible && !isFadingOut && !finish)
        {
            isFadingIn = true;
        }

        if (isFadingIn)
        {
            FadeIn();
        }

        if (isVisible)
        {
            timer += Time.deltaTime;
            if (timer >= duration)
            {
                isVisible = false;
                isFadingOut = true;
            }
        }

        if (isFadingOut)
        {
            FadeOut();
        }
    }

    void FadeIn()
    {
        if (tutorialImage != null)
        {
            currentAlpha = Mathf.Lerp(currentAlpha, 1f, fadeSpeed * Time.deltaTime);

            Color color = tutorialImage.color;
            color.a = currentAlpha;
            tutorialImage.color = color;

            if (Mathf.Abs(currentAlpha - 1f) < 0.01f)
            {
                currentAlpha = 1f;
                isFadingIn = false;
                isVisible = true;
                timer = 0f;
            }
        }

    }

    void FadeOut()
    {
        if (tutorialImage != null)
        {
            currentAlpha = Mathf.Lerp(currentAlpha, 0f, fadeSpeed * Time.deltaTime);

            Color color = tutorialImage.color;
            color.a = currentAlpha;
            tutorialImage.color = color;

            if (Mathf.Abs(currentAlpha - 0f) < 0.01f)
            {
                currentAlpha = 0f;
                isFadingOut = false;
                finish = true;

                // Capture if big painting was still active when tutorial ends
                wasBigPaintingActive = FindObjectOfType<HoldupUI>().isBigPaintingActive;
            }
        }
    }
}



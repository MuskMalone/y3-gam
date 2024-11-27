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


using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class FadeManager : Entity
{
    public static FadeManager Instance; // Singleton for easy access
    private Image fadeImage;

    private bool isFading = false; // Track if a fade is in progress
    private float fadeDuration = 0f;
    private float fadeElapsed = 0f;
    private float startAlpha;
    private float targetAlpha;
    private void Awake()
    {
        // Singleton pattern to access FadeManager from anywhere
        if (Instance == null)
        {
            Instance = this;
            //DontDestroyOnLoad(gameObject); // Optional: Keep across scenes
        }
        else
        {
            //Destroy(gameObject);
        }

        fadeImage = GetComponent<Image>();
    }

    private void Update()
    {
        if (isFading)
        {
            fadeElapsed += Time.deltaTime;


            float alpha = Mathf.Lerp(startAlpha, targetAlpha, fadeElapsed / fadeDuration);
            fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, alpha);


            if (fadeElapsed >= fadeDuration)
            {
                fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, targetAlpha);
                isFading = false;
            }
        }
    }
    public void FadeToBlack(float duration)
    {
        //StartCoroutine(Fade(1, duration)); // Fade to black
        StartFade(1, duration);
    }

    public void FadeFromBlack(float duration)
    {
        //StartCoroutine(Fade(0, duration)); // Fade back to transparent
        StartFade(0, duration);
    }

    private void StartFade(float targetAlphaValue, float duration)
    {
        if (isFading) return;

        fadeDuration = duration;
        fadeElapsed = 0f;
        startAlpha = fadeImage.color.a;
        targetAlpha = targetAlphaValue;
        isFading = true;
    }

}



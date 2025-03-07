﻿/******************************************************************************/
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

public class CorridorTransitionFade : Entity
{
    private PictureAlign pictureAlign;

    public bool isFading = false;
    private float fadeDuration = 3f;
    private float fadeElapsed = 0f;
    private float startAlpha = 0f; // Initial alpha
    private float targetAlpha = 1f; // Target alpha

    public Entity fadeImageEntity; // Entity containing the fade Image
    private Image fadeImage; // Image used for the fade effect
    private bool startFading = false;

    // Start is called before the first frame update
    void Start()
    {
        InternalCalls.HideCursor();
        pictureAlign = FindObjectOfType<PictureAlign>();
        if (pictureAlign == null) Debug.LogError("PictureAlign component not found!");
        if (fadeImageEntity != null)
        {
            fadeImage = fadeImageEntity.GetComponent<Image>();
        }

        if (fadeImage != null)
        {
            //Debug.Log("fadeimage is here");
            fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, startAlpha);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (pictureAlign.startFade && !startFading)
        {
            fadeElapsed = 0f;
            isFading = true;
            startFading = true;
        }

        if (isFading)
        {
            fadeElapsed += Time.deltaTime;
            float alpha = Mathf.Lerp(startAlpha, targetAlpha, fadeElapsed / fadeDuration);

            if (fadeImage != null)
            {
                fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, alpha);
            }

            float volume = Mathf.Lerp(0.6f, 0.0f, fadeElapsed / fadeDuration); // Fade volume from 1.0 to 0.0
            //uint playerTag = InternalCalls.FindParentByTag("Player");
            InternalCalls.SetSoundVolume(mEntityID, "BGM", volume); // Apply the volume fade
            if (fadeElapsed >= fadeDuration)
            {
                isFading = false;

            }
        }
    }

}



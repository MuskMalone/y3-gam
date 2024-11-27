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
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;

public class WASDTutorial : Entity
{
    // Start is called before the first frame update
    private Image tutorialImage;

    private TutorialCutscene tutorialCutscene;

    public float fadeSpeed = 0.5f;
    private float targetAlpha = 1f;

    void Start()
    {
        tutorialImage = GetComponent<Image>();
        tutorialCutscene = FindObjectOfType<TutorialCutscene>();

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
       if(tutorialCutscene.cutsceneFinish)
        {
            Color color = tutorialImage.color;
            color.a = 1f;
            tutorialImage.color = color;
        }
    }
}



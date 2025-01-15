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
using System.Threading;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;

public class TutorialCutscene : Entity
{


    public Entity mother;
    public Entity openDoor;
    public Entity closeDoor;
    public Entity glow;

    private PlayerMove playerMove;

    private bool firstCutscenePlaying = false;
    private bool secondCutscenePlaying = false;
    public bool cutsceneFinish = false;

    //dialogue
    private TutorialDialogue tutorialDialogue;
    public string[] firstCutsceneDialogue;
    public string[] secondCutsceneDialogue;
    public string[] thirdCutsceneDialogue;
    // Start is called before the first frame update
    void Start()
    {
        tutorialDialogue = FindObjectOfType<TutorialDialogue>();
        playerMove = FindObjectOfType<PlayerMove>();

        if (mother != null)
        {
            mother.SetActive(true);
        }
        else
        {
            Debug.LogError("mother entity is missing.");
        }

        if (openDoor != null)
        {
            openDoor.SetActive(true);
        }
        else
        {
            Debug.LogError("openDoor entity is missing.");
        }

        if (closeDoor != null)
        {
            closeDoor.SetActive(false);
        }
        else
        {
            Debug.LogError("closeDoor entity is missing.");
        }

        if (glow != null)
        {
            glow.SetActive(true);
        }
        else
        {
            Debug.LogError("glow entity is missing.");
        }

        StartFirstCutscene();
    }

    // Update is called once per frame
    void Update()
    {
        if (firstCutscenePlaying && Input.GetMouseButtonDown(0) && tutorialDialogue.isLineComplete) // Left mouse click
        {
            EndFirstCutscene();
        }
        else if (secondCutscenePlaying && Input.GetMouseButtonDown(0) && tutorialDialogue.isLineComplete) // Left mouse click
        {
            EndSecondCutscene();
        }
    }

    void StartFirstCutscene()
    {
        playerMove.FreezePlayer();
        firstCutscenePlaying = true;
        tutorialDialogue.SetDialogue(firstCutsceneDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Neutral });
    }

    void EndFirstCutscene()
    {
        mother.SetActive(false);
        firstCutscenePlaying = false;

        StartSecondCutscene();
    }

    void StartSecondCutscene()
    {
        secondCutscenePlaying = true;
        tutorialDialogue.SetDialogue(secondCutsceneDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Shocked });
    }

    void EndSecondCutscene()
    {
        closeDoor.SetActive(true);
        openDoor.SetActive(false);
        glow.SetActive(false);
        tutorialDialogue.SetDialogue(thirdCutsceneDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Surprised });
        playerMove.UnfreezePlayer();
        secondCutscenePlaying = false;
        cutsceneFinish = true;
    }

}
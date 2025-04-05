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
using System.Numerics;
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
    public BootupText bootup;

    private PlayerMove playerMove;

    private bool firstCutscenePlaying = false;
    private bool secondCutscenePlaying = false;
    public bool cutsceneFinish = false;
    private bool initialTrigger = true;

    //dialogue
    private TutorialDialogue tutorialDialogue;
    public string[] firstCutsceneDialogue;
    public string[] secondCutsceneDialogue;
    public string[] thirdCutsceneDialogue;
    private bool isFirstDialogueActive;
    private bool isSecondDialogueActive;  
    private bool isThirdDialogueActive;
    private bool hasPlayedLine0Sound = false;
    private bool hasPlayedLine1Sound = false;
    private bool hasPlayedLine2Sound = false;

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
    }

    // Update is called once per frame
    void Update()
    {
    if (initialTrigger && bootup.IsBootupTextCompleted())
    {
      StartFirstCutscene();
      initialTrigger = false;
    }

    if (firstCutscenePlaying && Input.GetMouseButtonDown(0) && tutorialDialogue.isLineComplete) // Left mouse click
      {
          EndFirstCutscene();
      }
      else if (secondCutscenePlaying && Input.GetMouseButtonDown(0) && tutorialDialogue.isLineComplete) // Left mouse click
      {
          EndSecondCutscene();
      }

        if (isFirstDialogueActive)
        {
            if (tutorialDialogue.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "LT_5");
                hasPlayedLine0Sound = true;
                isFirstDialogueActive = false;
            }

        }

        if(isSecondDialogueActive)
        {
            if (tutorialDialogue.CurrentLineIndex == 0 && !hasPlayedLine1Sound)
            {
                InternalCalls.StopSound(mEntityID, "LT_5");
                InternalCalls.PlaySound(mEntityID, "LT_6");
                hasPlayedLine1Sound = true;
                isSecondDialogueActive = false;
            }
        }

        if(isThirdDialogueActive)
        {
            if (tutorialDialogue.CurrentLineIndex == 0 && !hasPlayedLine2Sound)
            {
                InternalCalls.StopSound(mEntityID, "LT_6");
                InternalCalls.PlaySound(mEntityID, "LT_7");
                hasPlayedLine2Sound = true;
                isThirdDialogueActive = false;
            }
        }
    }

    void StartFirstCutscene()
    {
        playerMove.FreezePlayer();
        firstCutscenePlaying = true;
        tutorialDialogue.SetDialogue(firstCutsceneDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Neutral });
        isFirstDialogueActive = true;
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
        isSecondDialogueActive = true;
    }

    void EndSecondCutscene()
    {
        closeDoor.SetActive(true);
        openDoor.SetActive(false);
        glow.SetActive(false);
        tutorialDialogue.SetDialogue(thirdCutsceneDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Neutral });
        isThirdDialogueActive = true;
        secondCutscenePlaying = false;
        cutsceneFinish = true;
    }
}
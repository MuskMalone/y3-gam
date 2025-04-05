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

public class TutorialDoorDialogueTrigger : Entity
{
    public PlayerInteraction playerInteraction;
    public Entity tutorialDialogue;
    private TutorialDialogue dialogueScript;
    public Entity interactDoorUI;
    public string[] lockedDoorDialogue;
    private bool isDialogueActive = false;
    private bool hasPlayedLine0Sound = false;
    private bool hasPlayedLine1Sound = false;
    // Start is called before the first frame update
    void Start()
    {
        dialogueScript = tutorialDialogue.FindScript<TutorialDialogue>();
    }

    // Update is called once per frame
    void Update()
    {
        bool doorHit = playerInteraction.RayHitString == "ClosedDoor";

        // If dialogue is playing, ensure the interaction UI is hidden.
        if (dialogueScript.isInDialogueMode)
        {
            interactDoorUI.SetActive(false);
        }
        else
        {
            // Only show the UI if the door is hit.
            interactDoorUI.SetActive(doorHit);
        }

        // Trigger dialogue if the door is clicked and no dialogue is currently active.
        if (Input.GetKeyTriggered(KeyCode.MOUSE_BUTTON_1) && doorHit && !dialogueScript.isInDialogueMode)
        {
            dialogueScript.SetDialogue(lockedDoorDialogue, new TutorialDialogue.Emotion[]
            {
            TutorialDialogue.Emotion.Surprised,
            TutorialDialogue.Emotion.Thinking
            });
            interactDoorUI.SetActive(false);
            isDialogueActive = true;
        }

        // Handle sound playback based on the current dialogue line.
        if (isDialogueActive)
        {
            if (dialogueScript.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "LT_8");
                hasPlayedLine0Sound = true;
            }
            else if (dialogueScript.CurrentLineIndex == 1 && !hasPlayedLine1Sound)
            {
                InternalCalls.StopSound(mEntityID, "LT_8");
                InternalCalls.PlaySound(mEntityID, "LT_9_VER2");
                hasPlayedLine1Sound = true;
                isDialogueActive = false;
            }
        }

        // Once the dialogue is finished, reset the sound flags so future door clicks can replay them.
        if (!dialogueScript.isInDialogueMode)
        {
            hasPlayedLine0Sound = false;
            hasPlayedLine1Sound = false;
        }
    }
}



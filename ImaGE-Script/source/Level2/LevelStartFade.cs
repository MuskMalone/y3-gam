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

public class LevelStartFade : Entity
{
  //public float fadeDuration = 3f;
  public Transition transition;

    private TutorialDialogue tutorialDialogue;
    public string[] introDialogueOne;
    private bool hasPlayedLine0Sound = false;
    //private bool isIntroDialogueActive = false;
    private float elapsedTime = 0f;
    public float fadeDuration = 3f;
    private bool dialogueTriggered = false;
    void Start()
  {
        tutorialDialogue = FindObjectOfType<TutorialDialogue>();
        transition.StartTransition(true, fadeDuration, Transition.TransitionType.TV_SWITCH);
        //elapsedTime += Time.deltaTime;
        //if(InternalCalls.GetCurrentScene() == "..\\Assets\\Scenes\\Level2.scn")
        //if (elapsedTime >= fadeDuration)
        //{

            //if (InternalCalls.GetCurrentScene() == "Level2")
            //{
            //    //Console.WriteLine("IS IT IN LEVEL 2 SCEN");
            //    if (tutorialDialogue != null)
            //    {
            //        //string[] finalLines = { "Mom..?" };
            //        //tutorialDialogue.Emotion[] emotions = { TutorialDialogue.Emotion.Sad }; // Choose an emotion if needed
            //        tutorialDialogue.SetDialogue(introDialogueOne, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Neutral });
            //        isIntroDialogueActive = true;
            //    }
            //    else
            //    {
            //        Debug.LogError("[LevelStartFade] Dialogue script is missing!");
            //    }
            //}
        //}


  }

    void Update()
    {

        if (InternalCalls.GetCurrentScene() == "Level2")
        {
            // Wait until the transition is finished
            if (!dialogueTriggered && transition.IsFinished())
            {
                // Optionally, add scene checks or other conditions here
                if (tutorialDialogue != null)
                {
                    tutorialDialogue.SetDialogue(introDialogueOne, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Neutral });
                    dialogueTriggered = true;
                }
                else
                {
                    Debug.LogError("[LevelStartFade] Dialogue script is missing!");
                }
            }

            // Play the dialogue sound when the dialogue is active and the dialogue is on the first line.
            if (dialogueTriggered && tutorialDialogue.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L2_1_VER1");
                hasPlayedLine0Sound = true;
            }
        }
    }
}

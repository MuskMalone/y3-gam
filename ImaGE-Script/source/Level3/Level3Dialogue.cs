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

public class Level3Dialogue : Entity
{
    // NOTE: THE DIALOGUE SCRIPT SHOULD BE ATTACHED TO ENTITY WITH TEXT COMPONENT

    // Public variables
    public enum Emotion
    {
        Neutral,
        Thinking,
        Shocked,
        Surprised,
        Happy,
        Disturbed,
        Sad
    }

    public float textSpeed;               // Time between each letter
    public bool isInDialogueMode;

    public PlayerMove playerMove;
    public Entity DialogueBox;
    public Entity NeutralTara;
    public Entity ThinkingTara;
    public Entity ShockedTara;
    public Entity SurprisedTara;
    public Entity HappyTara;
    public Entity DisturbedTara;
    public Entity SadTara;
    //public SpecialDialogue specialDialogue;

    // Private Variables
    private Emotion[] emotions;           // The emotions from the caller
    private int lineIndex = 0;            // Tracks the current line
    private string[] lines;               // The lines from the caller
    private int charIndex = 0;            // Tracks the current character index
    private float nextCharTime = 0f;      // Tracks the time for the next character
    private const float defaultFontSize = 0.006f;
    private bool specialSequence = true;

    public bool isLineComplete = false;

    private string textAudioName = "DefaultDialogueSound";
    private const string defaultFont = "..\\Assets\\Textures\\Sniglet-Regular.ttf";

    private bool audioUpdated = false;

    public Level3Dialogue() : base()
    {

    }

    void Start()
    {
        if (playerMove == null)
        {
            Debug.LogError("[Dialogue.cs] PlayerMove Script Entity not found!");
        }

        DeactivateAllEmotions();
        DialogueBox.SetActive(false);
        SetActive(false);
    }

    void Update()
    {
      if (InternalCalls.GetIsPaused())
      {
        InternalCalls.StopSound(mEntityID, textAudioName);
        InternalCalls.SetActive(mEntityID, false);
        DialogueBox.SetActive(false);
        DeactivateAllEmotions();
        return;
      }

      if (isInDialogueMode && !InternalCalls.GetIsPaused())
      {
        InternalCalls.SetActive(mEntityID, true);
        DialogueBox.SetActive(true);
        SetEmotion(emotions[lineIndex]);
        playerMove.FreezePlayer();
      }

    if (isInDialogueMode && IsActive() && DialogueBox.IsActive() &&
          Time.gameTime >= nextCharTime && charIndex < lines[lineIndex].Length)
        {
            InternalCalls.AppendText(mEntityID, lines[lineIndex][charIndex].ToString());
            charIndex++;
            nextCharTime = Time.gameTime + textSpeed;
            isLineComplete = false;
        }

        if (isInDialogueMode && IsActive() && DialogueBox.IsActive() && charIndex >= lines[lineIndex].Length)
        {
            // Line has ended, stop sound
            InternalCalls.StopSound(mEntityID, textAudioName);
            isLineComplete = true;
        }

        if (isInDialogueMode && IsActive() && DialogueBox.IsActive() &&
          Input.GetMouseButtonTriggered(0))
        {
            if (charIndex >= lines[lineIndex].Length)
            {
                NextLine();
            }
            else
            {
                SkipTyping();
            }
        }
    }

    // To be called by other scripts before starting the dialogue
    public void SetDialogue(string[] newLines, Emotion[] newEmotions, float textScale = defaultFontSize, string textAudio = "DefaultDialogueSound")
    {
        InternalCalls.SetTextScale(mEntityID, textScale);
        if (newLines.Length != newEmotions.Length)
        {
            Debug.LogError("Lines and emotions arrays must be the same length. " +
              "(Each line should have its own emotion)");
            return;
        }
        textAudioName = textAudio;
        lines = newLines;
        emotions = newEmotions;
        StartDialogue();
    }

    private void StartDialogue()
    {
        if (playerMove == null)
        {
            Debug.LogError("PlayerMove script not attached to Dialogue script.");
            return;
        }

        if (lines == null || lines.Length == 0)
        {
            Debug.LogError("Dialogue lines are not set.");
            return;
        }

        InternalCalls.PlaySound(mEntityID, textAudioName);
        playerMove.FreezePlayer();
        lineIndex = 0;
        InternalCalls.SetText(mEntityID, string.Empty);
        isInDialogueMode = true;
        DialogueBox.SetActive(true);
        SetActive(true);
        SetEmotion(emotions[lineIndex]);
        charIndex = 0;                      // Reset character index for typing effect
        nextCharTime = Time.gameTime;       // Start typing immediately
    }

    private void EndDialogue()
    {
        isLineComplete = true;
        InternalCalls.StopSound(mEntityID, textAudioName);
        playerMove.UnfreezePlayer();
        DialogueBox.SetActive(false);
        SetActive(false);
        isInDialogueMode = false;

        // The first sequence
        //if (specialSequence)
        //{
        //    specialDialogue.StartSilhouetteSequence();
        //    specialSequence = false;
        //}
    }

    private void SkipTyping()
    {
        InternalCalls.StopSound(mEntityID, textAudioName);
        InternalCalls.SetText(mEntityID, lines[lineIndex]);
        charIndex = lines[lineIndex].Length;
        isLineComplete = true;
    }

    private void NextLine()
    {
        DeactivateAllEmotions();
        InternalCalls.PlaySound(mEntityID, textAudioName);

        if (lineIndex < lines.Length - 1)
        {
            lineIndex++;
            SetEmotion(emotions[lineIndex]);
            InternalCalls.SetText(mEntityID, string.Empty);
            charIndex = 0;                                      // Reset character index for new line
            nextCharTime = Time.gameTime;                       // Start typing new line immediately
            isLineComplete = false;
        }
        else
        {
            EndDialogue();
        }
    }

    private void SetEmotion(Emotion emotion)
    {
        DeactivateAllEmotions();

        switch (emotion)
        {
            case Emotion.Thinking:
                ThinkingTara.SetActive(true);
                break;
            case Emotion.Shocked:
                ShockedTara.SetActive(true);
                break;
            case Emotion.Surprised:
                SurprisedTara.SetActive(true);
                break;
            case Emotion.Happy:
                HappyTara.SetActive(true);
                break;
            case Emotion.Disturbed:
                DisturbedTara.SetActive(true);
                break;
            case Emotion.Sad:
                SadTara.SetActive(true);
                break;
            case Emotion.Neutral:
                NeutralTara.SetActive(true);
                break;
        }
    }

    private void DeactivateAllEmotions()
    {
        ThinkingTara.SetActive(false);
        ShockedTara.SetActive(false);
        SurprisedTara.SetActive(false);
        HappyTara.SetActive(false);
        DisturbedTara.SetActive(false);
        SadTara.SetActive(false);
        NeutralTara.SetActive(false);
    }


    public int CurrentLineIndex { get { return lineIndex; } }
}

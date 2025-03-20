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

public class PaintingAlignUILevel2 : Entity
{
  private HoldupUI holdupUIScript;
  private Level2Inventory level2inventoryScript;
  private PictureAlign pictureAlignScript;
  public Entity level2Dialogue;

  public Entity bigPaintingUI;
  public Entity smallPaintingUI;
  public Entity alignmentUI;
  public Entity leverTwo;
  public string[] lever2Dialogue;

  public bool isPainting = false;
  private PullLever leverTwoScript;
  private TutorialDialogue dialogueScript;

  // Start is called before the first frame update
  void Start()
  {
    holdupUIScript = FindObjectOfType<HoldupUI>();
    level2inventoryScript = FindObjectOfType<Level2Inventory>();
    pictureAlignScript = FindObjectOfType<PictureAlign>();

    bigPaintingUI.SetActive(false);
    smallPaintingUI.SetActive(false);
    alignmentUI.SetActive(false);

    leverTwoScript = leverTwo.FindScript<PullLever>();
    dialogueScript = level2Dialogue.FindScript<TutorialDialogue>();
  }

  // Update is called once per frame
  void Update()
  {
    // Ensure UI does not show if no valid item is selected
    if (level2inventoryScript.GetCurrentItem() == null || !level2inventoryScript.highlighted || !isPainting)
    {
      disableAlignUI();
      return;
    }

    if (holdupUIScript.isBigPaintingActive)
    {
      if (pictureAlignScript.IsAligned())
      {
        if (pictureAlignScript.GetCurrentPainting() == "HexPaintingDestructible2to1")
        {
          if (leverTwoScript.LeverPulled())
          {
            pictureAlignScript.preventAlignment = false;
          }
          else if (Input.GetMouseButtonTriggered(0) && !dialogueScript.isInDialogueMode)
          {
            dialogueScript.SetDialogue(lever2Dialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Thinking });
          }
        }

        bigPaintingUI.SetActive(false);
        smallPaintingUI.SetActive(false);
        alignmentUI.SetActive(true);
      }
      else
      {
        bigPaintingUI.SetActive(true);
        smallPaintingUI.SetActive(false);
        alignmentUI.SetActive(false);
      }
    }
    else
    {
      bigPaintingUI.SetActive(false);
      smallPaintingUI.SetActive(true);
      alignmentUI.SetActive(false);
    }
  }


  public void disableAlignUI()
  {
    bigPaintingUI.SetActive(false);
    smallPaintingUI.SetActive(false);
    alignmentUI.SetActive(false);
  }

}

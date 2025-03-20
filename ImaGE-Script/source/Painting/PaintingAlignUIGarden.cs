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

public class PaintingAlignUIGarden : Entity
{
  public Entity bigPaintingUI;
  public Entity smallPaintingUI;
  public Entity alignmentUI;
  public Fragment fragment;
  public Dialogue dialogueScript;
  public string[] collectFragmentDialogue;

  public bool isPainting = false;

  private HoldupUI holdupUIScript;
  private Inventory inventoryScript;
  private PictureAlign pictureAlignScript;

  // Start is called before the first frame update
  void Start()
  {
    holdupUIScript = FindObjectOfType<HoldupUI>();
    inventoryScript = FindObjectOfType<Inventory>();
    pictureAlignScript = FindObjectOfType<PictureAlign>();

    bigPaintingUI.SetActive(false);
    smallPaintingUI.SetActive(false);
    alignmentUI.SetActive(false);
  }

  // Update is called once per frame
  void Update()
  {
    // Ensure UI does not show if no valid item is selected
    if (inventoryScript.GetCurrentItem() == null || !inventoryScript.highlighted || !isPainting)
    {
      disableAlignUI();
      return;
    }

    if (holdupUIScript.isBigPaintingActive)
    {
      if (pictureAlignScript.IsAligned())
      {
        if (pictureAlignScript.GetCurrentPainting() == "CorridorPainting")
        {
          // only allow alignment after fragment is collected
          if (fragment.IsFragmentCollected())
          {
            pictureAlignScript.preventAlignment = false;
          }
          else if (Input.GetMouseButtonTriggered(0) && !dialogueScript.isInDialogueMode)
          {
            dialogueScript.SetDialogue(collectFragmentDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Thinking });
            pictureAlignScript.preventAlignment = true;
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

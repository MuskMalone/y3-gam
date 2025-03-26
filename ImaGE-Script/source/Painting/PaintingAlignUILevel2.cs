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
  public string[] allPaintingsDialogue;

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
        string currPainting = pictureAlignScript.GetCurrentPainting();
        if (currPainting == "HexPaintingDestructible2to1")
        {
          // prevent alignment if lever2 hasn't been pulled
          if (leverTwoScript.LeverPulled())
          {
            pictureAlignScript.preventAlignment = false;
          }
          else if (Input.GetMouseButtonTriggered(0) && !dialogueScript.isInDialogueMode)
          {
            dialogueScript.SetDialogue(lever2Dialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Thinking });
          }
        }
        else if (currPainting.StartsWith("HexPaintingIndestructible"))
        {
          // prevent alignment if not all paintings are picked up
          if (level2inventoryScript.HasAllPaintings())
          {
            pictureAlignScript.preventAlignment = false;
          }
          else if(Input.GetMouseButtonTriggered(0) && !dialogueScript.isInDialogueMode)
          {
            dialogueScript.SetDialogue(allPaintingsDialogue, new TutorialDialogue.Emotion[] { TutorialDialogue.Emotion.Thinking });
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

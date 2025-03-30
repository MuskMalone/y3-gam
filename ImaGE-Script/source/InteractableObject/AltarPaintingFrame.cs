using IGE.Utils;
using System.Numerics;

public class AltarPaintingFrame : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity altarPaintingFrameUI;
  public string[] paintingDialogue;
  public Dialogue dialogueSystem;

  private bool paintingFlag = false;
    private bool hasPlayedLine0Sound = false;
    private bool hasPlayedLine1Sound = false;
    private bool isAltarPaintingActive = false;
    void Start()
  {
    altarPaintingFrameUI?.SetActive(false);
  }

  void Update()
  {
    bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetMouseButtonTriggered(0) && isPaintingHit && !paintingFlag && !dialogueSystem.isInDialogueMode)
    {
      dialogueSystem.SetDialogue(paintingDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Neutral, Dialogue.Emotion.Thinking });
      paintingFlag = true;
      isAltarPaintingActive |= true;
      return;
    }

    altarPaintingFrameUI.SetActive(isPaintingHit);

    if (!isPaintingHit)
    {
      paintingFlag = false;
    }

        if (isAltarPaintingActive)
        {
            if (dialogueSystem.CurrentLineIndex == 0 && !hasPlayedLine0Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L1_6_VER2");
                hasPlayedLine0Sound = true;
            }
            else if (dialogueSystem.CurrentLineIndex == 1 && !hasPlayedLine1Sound)
            {
                InternalCalls.PlaySound(mEntityID, "L1_7_VER2");
                hasPlayedLine1Sound = true;
                // Optionally disable the dialogue active flag if there are no more sounds
                isAltarPaintingActive = false;
            }
        }
    }
}

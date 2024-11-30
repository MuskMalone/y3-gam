using IGE.Utils;
using System.Numerics;

public class AltarPaintingFrame : Entity
{
  public PlayerInteraction playerInteraction;
  public Entity altarPaintingFrameUI;
  public string[] paintingDialogue;
  public Dialogue dialogueSystem;

  private bool paintingFlag = false;

  void Start()
  {
    altarPaintingFrameUI?.SetActive(false);
  }

  void Update()
  {
    bool isPaintingHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
    if (Input.GetMouseButtonTriggered(0) && isPaintingHit && !paintingFlag)
    {
      dialogueSystem.SetDialogue(paintingDialogue, new Dialogue.Emotion[] { Dialogue.Emotion.Neutral, Dialogue.Emotion.Thinking });
      paintingFlag = true;
      return;
    }

    altarPaintingFrameUI.SetActive(isPaintingHit);

    if (!isPaintingHit)
    {
      paintingFlag = false;
    }
  }
}

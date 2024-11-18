using IGE.Utils;
public class SpecialDialogue : Entity
{
  public string introMessage;
  public string[] initialDialogue;
  public float fadeDuration = 0.5f;
  public float typewriterSpeed = 0.3f;
  private bool triggerInitialDialogue = true;

  public Dialogue dialogueSystem;

  public SpecialDialogue() : base()
  {

  }

  void Start()
  {

  }

  void Update()
  {
    if (triggerInitialDialogue)
    {
      dialogueSystem.SetDialogue(initialDialogue, 
        new Dialogue.Emotion[] { Dialogue.Emotion.Neutral, Dialogue.Emotion.Thinking, 
          Dialogue.Emotion.Shocked, Dialogue.Emotion.Surprised, Dialogue.Emotion.Happy});
      triggerInitialDialogue = false;
    }
  }
}

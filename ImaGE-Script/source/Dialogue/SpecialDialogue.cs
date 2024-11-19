using IGE.Utils;
public class SpecialDialogue : Entity
{
  public string introMessage;
  public string[] initialDialogue;
  public float fadeDuration = 0.5f;
  public float typewriterSpeed = 0.3f;

  public Dialogue dialogueSystem;
  // Temp workaround for lack of Entity[]
  private Entity[] BeginningSilhouetteSequence;
  public Entity MotherSilhouette;
  public Entity MotherHandsBySideOne;
  public Entity MotherHandsBySideTwo;
  public Entity MotherPickup;

  private bool triggerInitialDialogue = true;

  public SpecialDialogue() : base()
  {

  }

  void Start()
  {
    // Temp workaround for lack of Entity[]
    BeginningSilhouetteSequence = new Entity[]
    {
        MotherSilhouette,
        MotherHandsBySideOne,
        MotherHandsBySideTwo,
        MotherPickup
    };
    MotherSilhouette.SetActive(false);
    MotherHandsBySideOne.SetActive(false);
    MotherHandsBySideTwo.SetActive(false);
    MotherPickup.SetActive(false);
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

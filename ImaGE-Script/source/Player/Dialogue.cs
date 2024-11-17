using IGE.Utils;
using System.Numerics;

public class Dialogue : Entity
{
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

  public float fadeDuration = 0.5f; // Duration for the fade-out effect
  public float typewriterSpeed = 0.3f; // Speed of the typewriter effect (time between each letter)

  public string[] initialDialogue;

  public PlayerMove playerMove;
  public Entity DialogueBox;
  public Entity NeutralTara;
  public Entity ThinkingTara;
  public Entity ShockedTara;
  public Entity SurprisedTara;
  public Entity HappyTara;
  public Entity DisturbedTara;
  public Entity SadTara;

  private Emotion[] emotions;

  public Dialogue() : base()
  {
      
  }

  void Start()
  {
    DeactivateAllEmotions();
  }

  void Update()
  {
    //FreezePlayer();
  }

  void DeactivateAllEmotions()
  {
    ThinkingTara.SetActive(false);
    ShockedTara.SetActive(false);
    SurprisedTara.SetActive(false);
    HappyTara.SetActive(false);
    DisturbedTara.SetActive(false);
    SadTara.SetActive(false);
    NeutralTara.SetActive(false);
  }

  public void FreezePlayer()
  {
    if (playerMove != null)
    {
      playerMove.canMove = false;
      playerMove.canLook = false;
    }
  }

  public void UnfreezePlayer()
  {
    if (playerMove != null)
    {
      playerMove.canMove = true;
      playerMove.canLook = true;
    }
  }
}
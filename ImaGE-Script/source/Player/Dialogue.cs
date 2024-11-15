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

  //public string[] initialDialogue;
  // Workaround for lack of string[] in current engine
  // Initial Dialogue
  public string initalDialogue_FirstLine;
  public string initalDialogue_SecondLine;
  public string initalDialogue_ThirdLine;
  public string initalDialogue_FourthLine;
  public string initalDialogue_FifthLine;

  private PlayerMove playerMove;

  public Dialogue() : base()
  {
      
  }

  void Start()
  {

  }

  void Update()
  {

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
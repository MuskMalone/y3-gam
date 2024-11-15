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
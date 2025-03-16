using IGE.Utils;
using System;
using System.Numerics;
using System.Text;
using static System.TimeZoneInfo;

public class GameEndTrigger : Entity
{
  public Transition transition;
  public PlayerMove playerMove;
  public float transitionTime = 2f;

  private float transitionTimer = 0f;
  private bool triggered = false;

  public GameEndTrigger() : base()
  {

  }

  void Start()
  {

  }

  void Update()
  {
    if (InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
    {
      triggered = true;
      playerMove.FreezePlayer();
      transition.StartTransition(false, transitionTime, Transition.TransitionType.TV_SWITCH);
    }

    if (triggered)
    {
      transitionTimer += Time.deltaTime;
      if (transitionTimer > transitionTime)
      {
        InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\MonstersAteMyMom.scn");
      }
    }
  }
}
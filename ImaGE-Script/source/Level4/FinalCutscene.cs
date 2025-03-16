using IGE.Utils;
using System;
using System.Numerics;
using System.Text;
using static System.TimeZoneInfo;

public class FinalCutscene : Entity
{
  public Transition transition;
  public float transitionTime = 2f;
  public Entity mom;
  public Entity monster;

  private bool initial = true;
  private float transitionTimer = 0f;
  private bool triggered = false;

  public FinalCutscene() : base()
  {

  }

  void Start()
  {
    mom.SetActive(true);
    monster.SetActive(false);
    transition.StartTransition(true, transitionTime, Transition.TransitionType.TV_SWITCH);
  }

  void Update()
  {
    if (initial)
    {
      transitionTimer += Time.deltaTime;
      if (transitionTimer > transitionTime)
      {
        transition.StartTransitionInOut(0.5f, Transition.TransitionType.FADE);
        triggered = true;
        mom.SetActive(false);
        monster.SetActive(true);
        initial = false;
        transitionTimer = 0f;
      }
    }

    if (triggered)
    {
      transitionTimer += Time.deltaTime;
      if (transitionTimer > transitionTime * 2f)
      {
        InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\Credits.scn");
      }
    }
  }
}
using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class MomPaintingCutscene : Entity
{
  public Entity cutmum;
  public Entity cutmumhead;
  public Entity cutmumtorso;
  public Entity cutmumLArm;
  public Entity cutmumLForearm;
  public Entity cutmumLHand;
  public Entity cutmumLThigh;
  public Entity cutmumLLeg;
  public Entity cutmumRArm;
  public Entity cutmumRForearm;
  public Entity cutmumRHand;
  public Entity cutmumRThigh;
  public Entity cutmumRLeg;

  public Entity puppet;
  public Entity light;
  public PlayerMove playerMove;
  public Entity cutscene;
  public Fragment fragment;
  private enum State
  { 
    WAITING,
    PLAYING,
    TURNING, 
    DONE,
  }
  private State state = State.WAITING;

  private float timeElapsed = 0f;
  private float delayAfterCollection = 1f;
  private float videoLength = 10f;
  private float turnSpeed = 0.3f;
  private bool firstframe = false; 
  MomPaintingCutscene() : base()
  {
  }
  // Start is called before the first frame update
  void Start()
  {
    firstframe = false;
    Global.shouldFollow = false;
    InternalCalls.SetChildActiveToFollowParent(puppet.mEntityID, false);
  }
  void Update()
  {
    switch (state) { 
      case State.WAITING:
        if (fragment.IsFragmentAnimationOver())
        {
          if (!cutscene.IsActive())
          {
            cutscene.SetActive(true);
            cutscene.GetComponent<Video>().ClearFrame();
          }

          timeElapsed += Time.deltaTime;
          if (timeElapsed >= delayAfterCollection)
          {
            timeElapsed = 0f;
            cutscene.GetComponent<Video>().Play();
            state = State.PLAYING;
            puppet.SetActive(true);
            //InternalCalls.SetChildActiveToFollowParent(puppet.mEntityID, true);
            playerMove.FreezePlayer();
          }
        }

        break;
      case State.PLAYING:
        timeElapsed += Time.deltaTime;
        if (timeElapsed >= videoLength)
        {
          cutscene.SetActive(false);
          timeElapsed = 0f;
          state = State.TURNING;

        }
        break;
      case State.TURNING:
        if (!firstframe) {
          InternalCalls.SetMotionType(cutmumhead.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumtorso.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumLArm.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumLForearm.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumLHand.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumLThigh.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumLLeg.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumRArm.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumRForearm.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumRHand.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumRThigh.mEntityID, 0);
          InternalCalls.SetMotionType(cutmumRLeg.mEntityID, 0);
          InternalCalls.PlaySound(cutmum.mEntityID, "..\\Assets\\Audio\\PuppetSquishing_SFX.wav");
          firstframe = true;
        }
        timeElapsed += Time.deltaTime;
        if (timeElapsed >= turnSpeed)
        {
          cutscene.SetActive(false);
          timeElapsed = 0f;
          state = State.DONE;
          playerMove.UnfreezePlayer();
          Global.shouldFollow = true;
          InternalCalls.SetLightRange(light.mEntityID, 26f);
        }
        break;
      case State.DONE:
        break;
    }
  }
}

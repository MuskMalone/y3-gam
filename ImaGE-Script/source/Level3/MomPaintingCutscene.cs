using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class MomPaintingCutscene : Entity
{
  public PlayerMove playerMove;
  public Entity cutscene;
  public Fragment fragment;
  private enum State
  { 
    WAITING,
    PLAYING,
    DONE,
  }
  private State state = State.WAITING;

  private float timeElapsed = 0f;
  private float delayAfterCollection = 0.5f;
  private float videoLength = 10f;
  MomPaintingCutscene() : base()
  {
  }
  // Start is called before the first frame update
  void Start()
  {
  }
  void Update()
  {
    switch (state) { 
      case State.WAITING:
        if (fragment.IsFragmentAnimationOver())
        {
          timeElapsed += Time.deltaTime;
          if (timeElapsed >= delayAfterCollection)
          {
            timeElapsed = 0f;
            cutscene.SetActive(true);
            InternalCalls.PlayVideo(cutscene.mEntityID);
            state = State.PLAYING;
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
          state = State.DONE;
          playerMove.UnfreezePlayer();
        }
        break;
      case State.DONE:
        break;
    }
  }
}

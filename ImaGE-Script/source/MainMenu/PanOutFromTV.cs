using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

// to be placed on player
public class PanOutFromTV : Entity
{
  public Entity video;
  public Entity bgmEntity;
  public Transition transition;
  public float delayTillVideoStart, delayTillPanOut;
  public string panOutAnimName;
  public bool playIntroSequence;

  public Entity skipText;
  public float skipTextShownDuration;

  enum State
  {
    AWAKE,
    TV_FOCUS,
    PAN_OUT,
    PANNING,
    ENDED
  }
  private State currState = State.AWAKE;
  private float timeElapsed = 0f, skipTimeElapsed = 0f;

  // Start is called before the first frame update
  void Start()
  {
    // start with black screen
    video.GetComponent<Video>().ClearFrame();
    skipText.SetActive(false);
  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      case State.AWAKE:
        {
          if (transition.IsFinished())
          {
            // for intro sequence, keep the camera still and
            // don't play the bgm yet
            if (playIntroSequence)
            {
              currState = State.TV_FOCUS;
            }
            else
            {
              InternalCalls.PlaySound(bgmEntity.mEntityID, "MainMenuBGM");
              Video vid = video.GetComponent<Video>();
              vid.Play();
              vid.SetVolume(0.7f);
              GetComponent<Animation>().Play(panOutAnimName);
              currState = State.PANNING;
            }
          }

          break;
        }

      case State.TV_FOCUS:
        {
          timeElapsed += Time.deltaTime;

          if (Input.GetKeyTriggered(KeyCode.ESCAPE))
          {
            timeElapsed = delayTillVideoStart;
          }

          if (timeElapsed >= delayTillVideoStart)
          {
            timeElapsed = 0f;
            video.GetComponent<Video>().Play();
            currState = State.PAN_OUT;
          }

          break;
        }

      case State.PAN_OUT:
        {
          if (Input.anyKeyTriggered)
          {
            skipText.SetActive(true);
            skipTimeElapsed = 0f;
          }

          if (skipText.IsActive())
          {
            if (Input.GetKeyTriggered(KeyCode.ESCAPE))
            {
              timeElapsed = delayTillPanOut;
              skipText.SetActive(false);
            }
            else 
            {
              skipTimeElapsed += Time.deltaTime;

              if (skipTimeElapsed >= skipTextShownDuration)
              {
                skipText.SetActive(false);
              }
            }
          }
          
          timeElapsed += Time.deltaTime;

          if (timeElapsed >= delayTillPanOut)
          {
            InternalCalls.PlaySound(bgmEntity.mEntityID, "MainMenuBGM");
            GetComponent<Animation>().Play(panOutAnimName);
            video.GetComponent<Video>().SetVolume(0.7f);
            currState = State.PANNING;
          }

          break;
        }

      case State.PANNING:
        {
          if (GetComponent<Animation>().IsPlaying()) { return; }

          currState = State.ENDED;
          break;
        }

      default:
        return;
    }
  }

  public bool HasInitialSequenceEnded() { return currState == State.ENDED; }
}


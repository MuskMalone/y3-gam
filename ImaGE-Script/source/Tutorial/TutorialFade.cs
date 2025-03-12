using IGE.Utils;

public class TutorialFade : Entity
{
  private PictureAlign pictureAlign;
  private PlayerMove playerMove;

  public Transition transition;
  public bool isFading = false;
  public float fadeDuration = 3f;

  private bool startFading = false;
  private float fadeElapsed = 0f;

  void Start()
  {
    InternalCalls.HideCursor();
    pictureAlign = FindObjectOfType<PictureAlign>();
    if (pictureAlign == null) Debug.LogError("PictureAlign component not found!");
    playerMove = FindObjectOfType<PlayerMove>();
    if (playerMove == null) Debug.LogError("PlayerMove component not found!");
  }

  void Update()
  {
    if (pictureAlign.startFade && !startFading)
    {
      isFading = true;
      startFading = true;
      transition.StartTransition(false, fadeDuration, Transition.TransitionType.TV_SWITCH);
    }

    if (isFading)
    {
      playerMove.FreezePlayer();
      fadeElapsed += Time.deltaTime;

      float volume = Mathf.Lerp(0.6f, 0.0f, fadeElapsed / fadeDuration); // Fade volume from 1.0 to 0.0
      InternalCalls.SetSoundVolume(mEntityID, "BGM", volume); // Apply the volume fade

      if (fadeElapsed >= fadeDuration)
      {
        playerMove.UnfreezePlayer();
        isFading = false;
        InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\M3.scn");
      }
    }
  }
}



using IGE.Utils;
using System.Numerics;

public class EndSceneFadeToBlack : Entity
{
  public Entity fadeImage;
  public PlayerMove playerMove;
  private bool triggerFadeTransition = false;
  private bool isInFadeTransition = false;
  private float fadeTransitionTimer = 0f;
  private float fadeStartTime;
  public float fadeDuration = 3f;

  void Start()
  {
    
  }

  void Update()
  {
    // If player collides with entity, fade to black
    if (InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
    {
      triggerFadeTransition = true;
    }

    if (triggerFadeTransition)
    {
      StartFade();
    }

    if (isInFadeTransition)
    {
      float elapsed = Time.gameTime - fadeStartTime;
      fadeTransitionTimer = elapsed;

      float alpha = Mathf.Lerp(0f, 1f, fadeTransitionTimer / fadeDuration);
      InternalCalls.SetImageColor(fadeImage.mEntityID, new Vector4(1, 1, 1, alpha));

      if (fadeTransitionTimer >= fadeDuration)
      {
        EndFade();
      }
    }
  }

  private void StartFade()
  {
    triggerFadeTransition = false;
    isInFadeTransition = true;
    fadeImage.SetActive(true);
    playerMove.FreezePlayer();

    isInFadeTransition = true;
    fadeStartTime = Time.gameTime;
  }
  private void EndFade()
  {
    playerMove.UnfreezePlayer();
    isInFadeTransition = false;
    fadeImage.SetActive(false);

    // Change scenes at the end of the fade
    InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\ToBeContinued.scn");
  }
}

using IGE.Utils;
using System.Collections.Generic;
using System.Numerics;

public class BootupSequence : Entity
{
  public Entity image1;
  public Entity image2;
  public Entity image3;

  public Transition transition;
  public float timeBetweenTransition = 4f;
  public float timeOfTransition = 1f;

  private float timer = 0f;
  private List<Entity> images = new List<Entity>();
  private int currentImageIndex = 0;

  void Start()
  {
    images.Add(image1);
    images.Add(image2);
    images.Add(image3);

    foreach (Entity image in images)
    {
      image?.SetActive(false);
    }
  }

  void Update()
  {
    timer += Time.deltaTime;

    if (timer >= timeBetweenTransition && currentImageIndex < images.Count)
    {
      if (currentImageIndex >= 0)
      {
        if (transition.IsFinished())
          transition.StartTransition(false, timeOfTransition, Transition.TransitionType.FADE);

        SetAllImagesInactive();
        images[currentImageIndex].SetActive(true);
      }

      timer = 0f;
      currentImageIndex++;
    }

    if (timer >= timeBetweenTransition && currentImageIndex >= images.Count)
    {
      InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\mainmenu.scn");
    }
  }

  void SetAllImagesInactive()
  {
    foreach (Entity image in images)
    {
      image?.SetActive(false);
    }
  }
}
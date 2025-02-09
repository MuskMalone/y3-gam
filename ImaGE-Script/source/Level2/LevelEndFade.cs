using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class LevelEndFade : Entity
{
    public Entity fadeImageEntity; // Entity containing the fade Image
    private Image fadeImage; // Image used for the fade effect

    private float fadeDuration = 3f;
    private float fadeElapsed = 0f;
    private float startAlpha = 0f; // Fully transparent at start
    private float targetAlpha = 1f; // Fully black at end
    private bool isFading = false; // Only starts when triggered

    // Start is called before the first frame update
    void Start()
    {
        if (fadeImageEntity != null)
        {
            fadeImage = fadeImageEntity.GetComponent<Image>();
        }

        if (fadeImage != null)
        {
            fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, startAlpha);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (isFading)
        {
            fadeElapsed += Time.deltaTime;
            float alpha = Mathf.Lerp(startAlpha, targetAlpha, fadeElapsed / fadeDuration);

            if (fadeImage != null)
            {
                fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, alpha);
            }

            if (fadeElapsed >= fadeDuration)
            {
                isFading = false; // Stop fading after complete
                //OnFadeComplete(); // Optional function for scene transitions
            }
        }
    }

    // Call this function to start the fade out effect
    public void StartFadeOut()
    {
        if (!isFading)
        {
            fadeElapsed = 0f;
            isFading = true;
        }
    }

    //// Optional: Override this to add scene transition logic
    //private void OnFadeComplete()
    //{
    //    InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\NextScene.scn"); // Example scene transition
    //}
}

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

public class LevelStartFade : Entity
{
    public Entity fadeImageEntity; // Entity containing the fade Image
    private Image fadeImage; // Image used for the fade effect

    private float fadeDuration = 3f;
    private float fadeElapsed = 0f;
    private float startAlpha = 1f; // Fully black at start
    private float targetAlpha = 0f; // Fully transparent at end
    private bool isFading = true; // Starts fading immediately

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
            }
        }
    }
}

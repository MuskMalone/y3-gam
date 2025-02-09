#version 460 core

out vec4 fragColor;

uniform float u_TransitionProgress;  // Transition progress (0.0 - 1.0)
uniform vec4 u_FadeColor;            // Color to fade to
uniform bool u_FadeOut;              // True = fading out, False = fading in
uniform int u_TransitionType;        // 0 = Fade, 1 = TV Switch, 2 = Wipe

in vec2 fragTexCoord; // Fullscreen quad UV coordinates

// TV Switch Effect Constants
const float corner_harshness = 0.5;
const float corner_ease = 4.0;
const bool fade_dir_x = false;
const bool fade_dir_y = true;
const float fade_amount = 16.0;
const float fade_speed = 2.0;

void main() {
    float progress = clamp(u_TransitionProgress, 0.0, 1.0);
    vec4 finalColor;

    switch (u_TransitionType) {
    case 0: // **Fade Transition**
        if (u_FadeOut) {
            finalColor = mix(vec4(0.0), u_FadeColor, progress); // Alpha increases over time
        }
        else {
            finalColor = mix(u_FadeColor, vec4(0.0), progress); // Alpha decreases over time
        }
        break;

    case 1: { // **TV Switch Transition**
        float interpolation = pow(progress * fade_speed, fade_amount);
        float fade = max(interpolation, 1.0);

        float xx = (abs(fragTexCoord.x - 0.5) * corner_harshness) * (fade_dir_x ? fade : 1.0);
        float yy = (abs(fragTexCoord.y - 0.5) * corner_harshness) * (fade_dir_y ? fade : 1.0);
        float rr = (1.0 + pow((xx * xx + yy * yy), corner_ease));

        vec2 tuv = (fragTexCoord - 0.5) * rr + 0.5;
        tuv = clamp(tuv, 0.0, 1.0);

        // Black regions expanding, transparent where scene is visible
        if (tuv.x >= 1.0 || tuv.y >= 1.0 || tuv.x <= 0.0 || tuv.y <= 0.0) {
            finalColor = u_FadeColor;
        }
        else {
            finalColor = vec4(0.0); // Transparent where the scene should remain visible
        }

        break;  
    }

    case 2: // **Wipe Transition (Horizontal)**
        float wipeEffect = step(progress, fragTexCoord.x);
        finalColor = mix(vec4(u_FadeColor.rgb, 1.0), vec4(0.0), wipeEffect);
        break;

    default:
        finalColor = vec4(1.0, 0.0, 1.0, 1.0); // Debug: Purple if invalid transition type
        break;
    }

    fragColor = finalColor;
}

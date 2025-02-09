#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader
out vec4 fragColor;

uniform sampler2D u_ScreenTexture;  // Previous frame texture
uniform float u_TransitionProgress; // Transition progress (0.0 - 1.0)
uniform vec4 u_FadeColor; // Color to fade to (default black)
uniform bool u_FadeOut; // True = fading out, False = fading in

void main() {
    vec4 sceneColor = texture(u_ScreenTexture, fragTexCoord); // Sample screen texture

    // Interpolate between fade color and scene based on transition progress
    if (u_FadeOut) {
        fragColor = mix(sceneColor, u_FadeColor, u_TransitionProgress);
    }
    else {
        fragColor = mix(u_FadeColor, sceneColor, u_TransitionProgress);
    }
}

#version 460 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform vec2      u_Resolution;
uniform float     u_Strength;
uniform float     u_Time;
uniform sampler2D u_ScreenTex;

void mainImage(out vec4 O, in vec2 fragCoord) {
    vec2 uv = fragCoord / u_Resolution.xy;

    // === SCREEN SHAKE EFFECT (Intensity scales with u_Strength) ===
    float shakeAmount = u_Strength * 0.005; // Max offset range (~0.5% of screen)
    float shakeX = sin(u_Time * 30.0) * shakeAmount;
    float shakeY = cos(u_Time * 25.0) * shakeAmount;
    uv += vec2(shakeX, shakeY);
    uv = clamp(uv, vec2(0.0), vec2(1.0));

    // === VIGNETTE EFFECT ===
    vec2 center = vec2(0.5, 0.5);
    float dist = length(uv - center); // 0 at center, ~0.7 at corners

    float strengthFactor = mix(1.0, 5.0, u_Strength);
    float vignette = smoothstep(0.9, 0.2 / strengthFactor, dist);
    vignette = mix(1.0, vignette * vignette, u_Strength * 1.5);

    // Sample screen texture
    vec4 texColor = texture(u_ScreenTex, uv);

    // Apply vignette effect
    O = texColor * vignette;
}

void main() {
    mainImage(fragColor, fragTexCoord * u_Resolution);
}
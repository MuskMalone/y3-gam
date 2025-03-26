#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader
out vec4 fragColor;

uniform vec2      u_Resolution;          // viewport resolution (in pixels)
uniform float     u_Time;                // shader playback time (in seconds)
uniform sampler2D u_ScreenTex;           // input channel. XX = 2D/Cube

void mainImage(out vec4 O, in vec2 fragCoord) {
    // Normalize fragment coordinates to [0,1]
    vec2 uv = fragCoord / u_Resolution.xy;

    // Center of the screen (teleportation point)
    vec2 center = vec2(0.5, 0.5);

    // Calculate the distance from the center
    float dist = length(uv - center);

    // Increase the warp factor and power to make the effect more pronounced
    // We're now applying a stronger distortion
    float warpFactor = pow(dist, 1.2) * u_Time * 0.2;

    // Apply the warp by stretching the UV coordinates toward the center
    vec2 warpUV = uv + (center - uv) * warpFactor;

    // Sample the texture at the distorted coordinates
    vec4 texColor = texture(u_ScreenTex, warpUV);

    // Output the final color
    O = texColor;
}

void main() {
    mainImage(fragColor, fragTexCoord * u_Resolution);  // Convert fragTexCoord to screen space
}
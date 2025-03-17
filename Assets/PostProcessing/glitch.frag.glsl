#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader
out vec4 fragColor;

uniform vec2      u_Resolution;            // viewport resolution (in pixels)
uniform float     u_Time;                  // shader playback time (in seconds)
uniform int       u_Frame;                 // shader playback frame
uniform vec4      u_Mouse;                 // mouse pixel coords. xy: current (if MLB down), zw: click
uniform sampler2D u_ScreenTex;             // input channel. XX = 2D/Cube

// Random function to simulate glitch randomness
float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void mainImage(out vec4 O, vec2 U)
{
    vec2 R = u_Resolution.xy;

    // Introduce even smaller random offsets to simulate glitch displacement
    vec2 offset = vec2(rand(vec2(u_Time, U.y)) * 0.01, rand(vec2(U.x, u_Time)) * 0.01);
    
    // Reduce flickering intensity further to make it less aggressive
    float flicker = step(0.98, rand(vec2(u_Time, U.x + U.y)));  // Very high threshold for flickering
    if (flicker < 0.98) {  // Very rare flickering
        O = texture(u_ScreenTex, U - offset);  // Sample the screen texture with the small glitch offset
        return;
    }

    // Apply subtle color sampling with minimal glitch displacement
    O = texture(u_ScreenTex, U - offset);  // Sampling with a tiny displacement

    // Very subtle static noise (almost imperceptible)
    float noise = rand(vec2(u_Time, U.x)) * 0.005;  // Extremely light noise
    O.rgb += noise;  // Add minimal noise to the RGB values

    // Ensure the final color stays within valid bounds
    O = clamp(O, 0.0, 1.0);
}

void main(){
    mainImage(fragColor, fragTexCoord);
}